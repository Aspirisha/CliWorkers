#include "Worker.h"
const QHash<Worker::State, QString> Worker::stateToStr = { 
    { Worker::State::NOT_STARTED, "NOT_STARTED"},{ Worker::State::RUNNING, "RUNNING" },
    { Worker::State::PAUSED, "PAUSED" },{ Worker::State::STOPPED, "STOPPED" },
    { Worker::State::FINISHED, "FINISHED" } };

void Worker::start() {
    emit stateChanged(State::RUNNING);
    run();
}

void Worker::pause() {
    mutex.lock();
    emit stateChanged(State::PAUSED);
    resume_condition.wait(&mutex);
    emit stateChanged(State::RUNNING);
    mutex.unlock();
}

void Worker::resume() {
    resume_condition.wakeAll();
}

void Worker::stop() {
    stopped = true;
}

void Worker::on_finish() {
    stopped ? emit stateChanged(State::FINISHED) : emit stateChanged(State::STOPPED);
}

void Worker::command(QString command) {}