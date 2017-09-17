#pragma once

#include <random>
#include "Worker.h"

class QNetworkReply;
class QTimer;

class QuotesWorker : public Worker {
    Q_OBJECT
public:
    QuotesWorker(int requestIntervalMillis = 10000);
public slots:
    void command(QString command) override;
protected:
    void run() override;
private slots:
    void replyFinished(QNetworkReply *reply);
private:
    std::default_random_engine randomGen;
    QTimer *timer = nullptr;
    bool waiting_for_reply = false;
    int interval;
    static const QString quoteServerName;
};