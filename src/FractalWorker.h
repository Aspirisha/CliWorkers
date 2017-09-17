#pragma once
#include <QDir>
#include "Worker.h"

// see https://en.wikipedia.org/wiki/Barnsley_fern for details
class FractalWorker : public Worker {
    Q_OBJECT
public:
    FractalWorker(QString outputDirectory = defaultPath, int iterationsNumber = 100000, int writingFrequency = 5000, 
        int loopSleepMillis = 10, int width = int(defaultHeight / aspectRatio),
        int height = defaultHeight);
public slots:
    void run() override;
    void command(QString command) override;
public:
    static const QString defaultPath;
private:
    std::pair<int, int> convertPointToPixel(double x, double y);

    static constexpr double aspectRatio = 2.0669602249235095;
    static constexpr int defaultHeight = 1024;
    static constexpr int imageChannelsNum = 3;
    int iterationsNumber;
    int width;
    int height;
    QString outputDirectory;
    int writingPeriod;
    int loopSleepMillis;
};