#pragma once
#include <QObject>
#include <QMutex>
#include <QHash>
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

    static const QHash<State, QString> stateToStr;

    void resume(); // called externally
signals:
    void finished();
    void error(QString err);
    void stateChanged(State new_state);
    void processingStepChanged(QString);
    void message(QString msg);
public slots:
    void start();
    void pause(); 
    void stop();
    virtual void command(QString command);
protected slots:
    virtual void run() = 0;
protected:
    void on_finish();
    QWaitCondition resume_condition;
    QMutex mutex;
    bool stopped = false;
};

inline uint qHash(Worker::State key, uint seed) {
    return ::qHash(static_cast<uint>(key), seed);
}