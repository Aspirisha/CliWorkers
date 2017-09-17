#pragma once
#include <QTest>

class FractalWorkerTest : public QObject {
    Q_OBJECT
private slots:
    void sleepCommand();
};