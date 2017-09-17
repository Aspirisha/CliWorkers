#pragma once
#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class Worker : public QObject {
    Q_OBJECT
public:
    enum class State {
        NOT_STARTED,
        RUNNING,
        PAUSED,
        STOPPED,
        FINISHED
    };

    void resume(); // called externally
signals:
    void finished();
    void error(QString err);
    void state_changed(State new_state);
    void message(QString msg);
public slots:
    void start();
    void pause(); 
    void stop();
    virtual void run() = 0;
    virtual void command(QString command);
protected:
    void on_finish();
    QWaitCondition resume_condition;
    QMutex mutex;
    bool stopped = false;
};