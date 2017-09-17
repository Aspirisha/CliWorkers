#include <QSignalSpy>
#include "FractalWorkerTest.h"
#include "../src/FractalWorker.h"
void FractalWorkerTest::sleepCommand() {
    FractalWorker worker;

    worker.command("sleep 10000");
    QSignalSpy spy(&worker, SIGNAL(message(QString)));
    worker.command("interval");

    QCOMPARE(spy.count(), 1); 
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString() == "Current sleep interval is 10000");
    worker.command("sleep forever");
    QCOMPARE(spy.count(), 1);
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString() == "Couldn't parse sleep interval: forever");
}