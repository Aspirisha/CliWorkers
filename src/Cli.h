#pragma once

#include <QtCore>

class WorkerManager;
class ConsoleReader;
class Cli : public QObject
{
    Q_OBJECT
public:
    Cli(QObject *parent = 0);
public slots :
    void run();
    void onMessage(int id, QString msg);
private slots:
    void processUserInput(QString line);
    void exitRequest();
signals:
    void finished();
private:
    WorkerManager *worker_manager;
    ConsoleReader *console_reader;
    QThread* reader_thread;
};