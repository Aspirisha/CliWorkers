#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrlQuery>
#include <QThread>
#include <QDomDocument>
#include <QTextStream>
#include <Qtimer>
#include <random>
#include <chrono>
#include "QuotesWorker.h"

QuotesWorker::QuotesWorker(int request_interval_millis) : interval(request_interval_millis){
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &QuotesWorker::run);
}

void QuotesWorker::run() {
    if (!stopped) {
        timer->start(interval);
    }

    if (waiting_for_reply) return;

    QUrl url("http://api.forismatic.com/api/1.0/");
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery params;
    params.addQueryItem("method", "getQuote");
    params.addQueryItem("lang", "en");
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(1, 100000);
    int dice_roll = distribution(generator);  // generates number in the range 1..6 
    params.addQueryItem("key", QString::number(distribution(generator)));

    QByteArray data = params.query(QUrl::FullyEncoded).toUtf8();

    manager->post(request, data);
    waiting_for_reply = true;
}

void QuotesWorker::replyFinished(QNetworkReply *reply) {
    QString res = QString::fromUtf8(reply->readAll());
    QDomDocument doc;
    doc.setContent(res);

    QDomNodeList quote = doc.elementsByTagName("quoteText");
    QDomNodeList author = doc.elementsByTagName("quoteAuthor");
    if (!quote.isEmpty() && !author.isEmpty()) {
        QString text = quote.at(0).firstChild().nodeValue();
        QString author_name = author.at(0).firstChild().nodeValue();
        emit message(QString("[QuotesWorker]: \"%1\", %2").arg(text).arg(author_name));
    }
    waiting_for_reply = false;
}

void QuotesWorker::command(QString command) {
    QStringList command_parts = command.split(" ", QString::SkipEmptyParts);

    QTextStream out(stdout);
    if (command_parts.size() != 2 || command_parts.at(0) != "interval") return;

    bool ok;
    int interval = command_parts.at(1).toInt(&ok);
    if (!ok || interval < 1000) {
        emit message(QString("[QuotesWorker]: Interval %1 is too small").arg(interval));
        return;
    }

    this->interval = interval;
}