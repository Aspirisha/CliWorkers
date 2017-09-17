#include <QThread>
#include <QTextStream>
#include "WorkerManager.h"

WorkerManager::WorkerDescriptor::WorkerDescriptor(Worker *w, Worker::State state)
    : worker(w), state(state), thread(new QThread) {
    if (worker) {
        worker->moveToThread(thread.data());
        connect(worker, &Worker::finished, worker, &Worker::deleteLater);
        connect(worker, &Worker::finished, thread.data(), &QThread::quit);
    }
}

WorkerManager::WorkerManager(const QVector<Worker*> &workers) {
    for (int id = 1; id <= workers.size(); id++) {
        Worker *w = workers[id - 1];
        QString name = w->getName();
        connect(w, &Worker::message, [this, id, name](QString msg) {
            emit this->message(id, "[" + name + QString(" from thread-%1]: ").arg(id) + msg);
        });

        connect(w, &Worker::error, [this, id, name](QString msg) {
            emit this->error(id, "[" + name + QString(" from thread-%1]: ").arg(id) + msg);
        });

        connect(w, &Worker::stateChanged, [this, id](Worker::State newState) {
            this->workers[id - 1].state = newState;
        });

        connect(w, &Worker::processingStepChanged, [this, id](QString newStep) {
            this->workers[id - 1].processingStepDescription = newStep;
        });
        connect(w, &Worker::commandsReply, [this, id](QStringList commands) {
            emit workerCommands(id, commands);
        });
        this->workers.push_back(w);
    }
}

void WorkerManager::pause(int id) {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't pause thread %1: no thread with such id.").arg(id));
        return;
    }
    QMetaObject::invokeMethod(workers[id - 1].worker, "pause", Qt::QueuedConnection);
}

void WorkerManager::stop(int id) {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't stop thread %1: no thread with such id.").arg(id));
        return;
    }
    QMetaObject::invokeMethod(workers[id - 1].worker, "stop", Qt::QueuedConnection);
}

void WorkerManager::resume(int id) {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't resume thread %1: no thread with such id.").arg(id));
        return;
    }
    workers[id - 1].worker->resume();
}

void WorkerManager::start() {
    for (int i = 0; i < workers.size(); i++) {
        workers[i].thread->start();
        QMetaObject::invokeMethod(workers[i].worker, "start", Qt::QueuedConnection);
    }
}

void WorkerManager::stopAll() {
    for (WorkerDescriptor &wd : workers) {
        QMetaObject::invokeMethod(wd.worker, "stop", Qt::QueuedConnection);
        wd.thread->quit(); // this is normally done by worker emitting finished() signal, but who knows!
        wd.thread->wait(1000);
        if (!wd.thread->isFinished()) {
            wd.thread->terminate();
        }
    }
}

void WorkerManager::command(int id, QString command) {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't send command to thread %1: no thread with such id.").arg(id));
        return;
    } 
    QMetaObject::invokeMethod(workers[id - 1].worker, "command", Qt::QueuedConnection, Q_ARG(QString, command));
}

void WorkerManager::commands(int id) const {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't send command to thread %1: no thread with such id.").arg(id));
        return;
    } 
    QMetaObject::invokeMethod(workers[id - 1].worker, "commands", Qt::QueuedConnection);
}

void WorkerManager::status() const {
    QStringList statuses;
    for (int i = 0; i < workers.size(); i++) {
        statuses << QString("%1\t%2\t%3").arg(i + 1).arg(Worker::stateToStr[workers[i].state])
            .arg(workers[i].processingStepDescription);
    }
    emit statusReply(statuses);
}