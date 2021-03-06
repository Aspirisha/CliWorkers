#pragma once

#include <QVector>
#include <QSharedPointer>
#include "Worker.h"

enum class JobTypes {
    FRACTAL,
    HOUGH
};

class WorkerManager : public QObject {
    Q_OBJECT

    struct WorkerDescriptor {
        WorkerDescriptor(Worker *w = nullptr, Worker::State state = Worker::State::NOT_STARTED);

        Worker *worker;
        QSharedPointer<QThread> thread;
        Worker::State state;
        QString processingStepDescription = "Unknown";
    };
public:
    WorkerManager(const QVector<Worker*> &workers); // takes control over Workers, e.g. deletes it when needed
    void command(int id, QString command);
    void pause(int id);
    void resume(int id);
    void stop(int id);
    void status() const;
    void start();
    void stopAll();
    void commands(int id) const;
signals:
    void error(QString e) const;
    void error(int id, QString msg);
    void message(int id, QString msg);
    void statusReply(QStringList) const;
    void workerCommands(int id, QStringList commands) const;
private:
    QVector<WorkerDescriptor> workers;
};