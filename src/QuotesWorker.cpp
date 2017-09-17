#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrlQuery>
#include <QThread>
#include <QDomDocument>
#include <QTextStream>
#include <Qtimer>
#include <chrono>
#include "QuotesWorker.h"

const QString QuotesWorker::quoteServerName = "http://api.forismatic.com/api/1.0/";

QuotesWorker::QuotesWorker(int request_interval_millis) : interval(request_interval_millis), 
    randomGen(std::chrono::system_clock::now().time_since_epoch().count()){
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &QuotesWorker::run);
    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
}

void QuotesWorker::run() {
    if (stopped) {
        emit processingStepChanged("Stopped");
        onFinish();
        return;
    }

    timer->start(interval);
    if (waiting_for_reply) return;
    emit processingStepChanged("Requesting quote server");
    QUrl url(quoteServerName);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery params;
    params.addQueryItem("method", "getQuote");
    params.addQueryItem("lang", "en");
   
    std::uniform_int_distribution<int> distribution(1, 100000);
    params.addQueryItem("key", QString::number(distribution(randomGen)));

    QByteArray data = params.query(QUrl::FullyEncoded).toUtf8();

    manager->post(request, data);
    waiting_for_reply = true;
    emit processingStepChanged("Waiting for quotes server reply");
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
        emit message(QString("\"%1\", %2").arg(text).arg(author_name));
    }
    waiting_for_reply = false;
}

void QuotesWorker::command(QString command) {
    QStringList commandParts = command.split(" ", QString::SkipEmptyParts);

    if (commandParts.size() == 1 && commandParts.at(0) == "interval") {
        return emit message(QString("Current request interval is %1").arg(interval));
    }

    if (commandParts.size() != 2 || commandParts.at(0) != "interval") return;

    bool ok;
    int interval = commandParts.at(1).toInt(&ok);
   
    if (!ok) {
        return emit message(QString("Couldn't parse sleep interval: %1").arg(commandParts.at(1)));
    }

    if (interval < minRequestInterval) {
        return emit message(QString("Interval %1 is too small: should be at least %2")
            .arg(interval).arg(minRequestInterval));
    }

    this->interval = interval;
}

void QuotesWorker::commands() {
    QStringList commandParts;
    commandParts << "interval <N>\t request quote server for the new quote every <N> milliseconds";
    commandParts << "interval\t get current request interval in milliseconds";
    emit commandsReply(commandParts);
}

QString QuotesWorker::getName() const {
    return "QuotesWorker";
}