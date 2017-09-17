#include <iostream>
#include "Worker.h"

void Worker::start() {
    emit state_changed(State::RUNNING);
    run();
}

void Worker::pause() {
    mutex.lock();
    emit state_changed(State::PAUSED);
    resume_condition.wait(&mutex);
    emit state_changed(State::RUNNING);
    mutex.unlock();
}

void Worker::resume() {
    resume_condition.wakeAll();
}

void Worker::stop() {
    stopped = true;
}

void Worker::on_finish() {
    stopped ? emit state_changed(State::FINISHED) : emit state_changed(State::STOPPED);
}

void Worker::command(QString command) {}