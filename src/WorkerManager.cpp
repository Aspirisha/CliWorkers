#include <QThread>
#include "WorkerManager.h"

WorkerManager::WorkerManager(const QVector<Worker*> &workers) : workers(workers) {
    for (int id = 1; id <= workers.size(); id++) {
        QThread *t = new QThread(this);
        Worker *w = workers[id - 1];
        w->moveToThread(t);
        worker_threads.push_back(t);
        connect(w, &Worker::message, [this, id](QString msg) {
            emit this->message(id, msg);
        });
    }
}

void WorkerManager::pause(int id) {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't pause thread %1: no thread with such id.").arg(id));
        return;
    }
    QMetaObject::invokeMethod(workers[id - 1], "pause", Qt::QueuedConnection);
}

void WorkerManager::stop(int id) {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't stop thread %1: no thread with such id.").arg(id));
        return;
    }
    QMetaObject::invokeMethod(workers[id - 1], "stop", Qt::QueuedConnection);
}

void WorkerManager::resume(int id) {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't resume thread %1: no thread with such id.").arg(id));
        return;
    }
    workers[id - 1]->resume();
}

void WorkerManager::start() {
    for (int i = 0; i < workers.size(); i++) {
        worker_threads[i]->start();
        QMetaObject::invokeMethod(workers[i], "run", Qt::QueuedConnection);
    }
}

void WorkerManager::stopAll() {
    // TODO
}

void WorkerManager::command(int id, QString command) {
    if (id > workers.size() || id <= 0) {
        emit error(QString("Can't send command to thread %1: no thread with such id.").arg(id));
        return;
    }
    workers[id - 1]->command(command);
}
