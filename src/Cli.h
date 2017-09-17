#pragma once

#include <QtCore>

class WorkerManager;
class ConsoleReader;
class Cli : public QObject
{
    Q_OBJECT
public:
    Cli(int threadsNum, QObject *parent = 0);
public slots :
    void run();
    void onMessage(int id, QString msg);
private slots:
    void processUserInput(QString line);
    void exitRequest();
    void printStatus(QStringList status);
    void workerManagerError(QString msg);
    void workerError(int id, QString msg);
    void printWorkerCommands(int id, QStringList commands);
signals:
    void finished();
private:
    void printHelp();

    WorkerManager *worker_manager;
    ConsoleReader *console_reader;
    QThread* reader_thread;
};