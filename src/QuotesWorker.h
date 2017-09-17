#pragma once

#include "Worker.h"

class QNetworkReply;
class QTimer;

class QuotesWorker : public Worker {
    Q_OBJECT
public:
    QuotesWorker(int request_interval_millis = 10000);
public slots:
    void command(QString command);
protected:
    void run() override;
private slots:
    void replyFinished(QNetworkReply *reply);
private:
    QTimer *timer = nullptr;
    bool waiting_for_reply = false;
    int interval;
};