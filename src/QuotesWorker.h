#pragma once

#include <random>
#include "Worker.h"

class QNetworkReply;
class QTimer;


/// Infinite worker which spawns new quote one in while onto the console
class QuotesWorker : public Worker {
    Q_OBJECT
public:
    QuotesWorker(int requestIntervalMillis = 10000);
    QString getName() const override;
public slots:
    void command(QString command) override;
    void commands() override;
protected:
    void run() override;
private slots:
    void replyFinished(QNetworkReply *reply);
private:
    static const int minRequestInterval = 1000;
    std::default_random_engine randomGen;
    QTimer *timer = nullptr;
    bool waiting_for_reply = false;
    int interval;
    static const QString quoteServerName;
};