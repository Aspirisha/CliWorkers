#pragma once

#include <QVector>
#include "Worker.h"

enum class JobTypes {
    FRACTAL,
    HOUGH
};

class WorkerManager : public QObject {
    Q_OBJECT
public:
    WorkerManager(const QVector<Worker*> &workers);
    void command(int id, QString command);
    void pause(int id);
    void resume(int id);
    void stop(int id);
    void status() const;
    void start();
    void stopAll();
signals:
    void error(QString e);
    void message(int id, QString msg);
private slots:

private:
    QVector<Worker*> workers;
    QVector<QThread*> worker_threads;
};