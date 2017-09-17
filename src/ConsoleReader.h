#pragma once
#include <QtCore>

class ConsoleReader : public QObject {
    Q_OBJECT
signals:
    void gotLine(QString line);
    void exitRequest();
public:
    Q_INVOKABLE void run();
private:
    bool stopped = false;
};