#include <QTextStream>
#include "WorkerManager.h"
#include "ConsoleReader.h"
#include "QuotesWorker.h"
#include "FractalWorker.h"
#include "Cli.h"

Cli::Cli(int threadsNum, QObject *parent) : QObject(parent) {
    QVector<Worker*> workers;
    for (int i = 0; i < threadsNum; i++) {
        switch (i % 2) {
        case 0: {
            workers.push_back(new QuotesWorker);
            break;
        }
        case 1: {
            QString filepath = FractalWorker::defaultPath + QDir::separator() + QString("fractal-%1").arg(i + 1);
            workers.push_back(new FractalWorker(filepath));
            break;
        }
        }
    }

    worker_manager = new WorkerManager(workers);
    console_reader = new ConsoleReader;
    reader_thread = new QThread;
    connect(worker_manager, &WorkerManager::message, this, &Cli::onMessage);
    connect(worker_manager, &WorkerManager::statusReply, this, &Cli::printStatus);
    connect(worker_manager, static_cast<void (WorkerManager::*)(QString)>(&WorkerManager::error),
        this, &Cli::workerManagerError);
    connect(worker_manager, static_cast<void (WorkerManager::*)(int, QString)>(&WorkerManager::error),
        this, &Cli::workerError);
    connect(console_reader, &ConsoleReader::exitRequest, this, &Cli::exitRequest);
    connect(console_reader, &ConsoleReader::gotLine, this, &Cli::processUserInput);

    console_reader->moveToThread(reader_thread);
}


void Cli::run() {
    QTextStream cout(stdout);
    cout << "Welcome to workers cli!" << endl;
    cout << "> " << flush;
    worker_manager->start();
    reader_thread->start();
    QMetaObject::invokeMethod(console_reader, "run", Qt::QueuedConnection);
}

void Cli::processUserInput(QString command) {
    QRegExp regex("\\s*(\\w+)(\\s+([1-9][0-9]*))?");
    //QRegExp regex("pause 1");
    int pos;
    QTextStream cout(stdout);
    if ((pos = regex.indexIn(command, 0)) != -1) {
        if (regex.captureCount() == 3) {
            bool ok;
            int id = regex.cap(3).toInt(&ok);
            if (!ok && regex.cap(1) == "status") {
                worker_manager->status();
                cout << "> " << flush;
                return;
            }

            if (regex.cap(1) == "command") {
                worker_manager->command(id, command.mid(regex.cap(0).length()));
            } else if (regex.cap(1) == "pause") {
                worker_manager->pause(id);
            } else if (regex.cap(1) == "resume") {
                worker_manager->resume(id);
            } else if (regex.cap(1) == "stop") {
                worker_manager->stop(id);
            } else {
                cout << "Unknown command: " << regex.cap(1) << endl;
            }
        }
    }
    cout << "> " << flush;
}

void Cli::onMessage(int id, QString msg) {
    QTextStream cout(stdout);
    cout << "[thread-" << id << "]: " << msg << "\n> " << flush;
}

void Cli::exitRequest() {
    QTextStream cout(stdout);
    cout << "Good bye!" << endl;
    worker_manager->stopAll();
    reader_thread->quit();
    reader_thread->wait();
    emit finished();
}

void Cli::printStatus(QStringList status) {
    QTextStream cout(stdout);
    cout << "[Worker #]\t[State]\t[Processing Step]\n";
    for (QString s : status) {
        cout << s << endl;
    }
}

void Cli::workerManagerError(QString msg) {
    QTextStream cout(stdout);
    cout << "[ERROR] " << msg << endl;
}

void Cli::workerError(int id, QString msg) {
    QTextStream cout(stdout);
    cout << QString("[ERROR in thread-%1] ").arg(id) << msg << endl;
}