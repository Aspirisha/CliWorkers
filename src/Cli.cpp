#include <QTextStream>
#include "WorkerManager.h"
#include "ConsoleReader.h"
#include "QuotesWorker.h"
#include "Cli.h"

Cli::Cli(QObject *parent) : QObject(parent) {
    worker_manager = new WorkerManager({new QuotesWorker});
    console_reader = new ConsoleReader;
    reader_thread = new QThread;
    connect(worker_manager, &WorkerManager::message, this, &Cli::onMessage, Qt::QueuedConnection);
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
    QRegExp regex("\\s*(\\w+)\\s+([1-9][0-9]*)");
    //QRegExp regex("pause 1");
    int pos;
    if ((pos = regex.indexIn(command, 0)) != -1) {
        bool ok;
        int id = regex.cap(2).toInt(&ok);
        if (!ok) {
            return;
        }

        if (regex.cap(1) == "command") {
            worker_manager->command(id, command.right(regex.matchedLength()));
        } else if (regex.cap(1) == "pause") {
            worker_manager->pause(id);
        } else if (regex.cap(1) == "resume") {
            worker_manager->resume(id);
        } else if (regex.cap(1) == "stop") {
            worker_manager->stop(id);
        }
    }
    QTextStream cout(stdout);
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
    emit finished();
}