#include <functional>
#include <random>
#include <chrono>
#include <QVector>
#include <QTextStream>
#include <QCoreApplication>
#include <QThread>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "FractalWorker.h"


const QString FractalWorker::defaultPath = "fractals";

FractalWorker::FractalWorker(QString outputDirectory, int iterationsNumber, int writingPeriod, 
    int loopSleepMillis, int width, int height)
    : iterationsNumber(iterationsNumber), width(width), height(height), 
    outputDirectory(outputDirectory), 
    writingPeriod(qMax(1, writingPeriod)), loopSleepMillis(qMax(0, loopSleepMillis)) {}

void FractalWorker::run() {
    if (!QDir().mkpath(outputDirectory)) {
        stopped = true;
        emit error(QString("Couldn't create output directories: %1").arg(outputDirectory));
        on_finish();
        return;
    }
    double x = 0;
    double y = 0;
    auto f1 = [](double &x, double &y) {
        x = 0;
        y = 0.16 * y;
    };
    auto f2 = [](double &x, double &y) {
        double newX = 0.85*x + 0.04*y;
        y = -0.04 * x + 0.85 * y + 1.6;
        x = newX;
    };
    auto f3 = [](double &x, double &y) {
        double newX = 0.2*x - 0.26*y;
        y = 0.23 * x + 0.22 * y + 1.6;
        x = newX;
    };
    auto f4 = [](double &x, double &y) {
        double newX = -0.15*x + 0.28*y;
        y = 0.26 * x + 0.24 * y + 0.44;
        x = newX;
    };
    auto putPoint = [this](std::pair<int, int> pixel, uchar r, uchar g, uchar b, std::vector<uchar> &image) {
        int idx = (pixel.second * width + pixel.first) * imageChannelsNum;
        image[idx] = r;
        image[idx + 1] = g;
        image[idx + 2] = b;
    };

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    QVector<std::function<void(double&, double&)>> f = {f1, f2, f3, f4};
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(0, 3);
   

    std::vector<uchar> image(width * height * imageChannelsNum);
    for (int i = 0; i < iterationsNumber && !stopped; i++) {
        QCoreApplication::processEvents();
        if (loopSleepMillis > 0) {
            QThread::msleep(loopSleepMillis);
        }

        int funcNum = distribution(generator);
        f[funcNum](x, y);
        auto pixel = convertPointToPixel(x, y);
        putPoint(pixel, 0, 200, 0, image);
        emit processingStepChanged(QString("Finished %1 iterations").arg(i + 1));
        if ((i + 1) % writingPeriod == 0) {
            std::string imgPath = (outputDirectory + QDir::separator() + QString("iteration-%1.png").arg(i + 1)).toStdString();
            stbi_write_png(imgPath.c_str(), width, height, imageChannelsNum, image.data(), width * imageChannelsNum);
        }
    }

    on_finish();
}

std::pair<int, int> FractalWorker::convertPointToPixel(double x, double y) {
    const double minX = -2.1820;
    const double minY = 0;
    const double maxX = 2.6558;
    const double maxY = 9.9983;
    x -= minX;
    y -= minY;

    return{ x * width / (maxX - minX), (height - 1)  * (1 - y / (maxY - minY)) };
}

void FractalWorker::command(QString command) {
    QStringList commandParts = command.split(" ", QString::SkipEmptyParts);

    if (commandParts.size() != 2 || commandParts.at(0) != "sleep") return;
    bool ok;
    int interval = commandParts.at(1).toInt(&ok);
    if (!ok || interval < 0) {
        emit message(QString("[FractalWorker]: Interval %1 is too small").arg(interval));
        return;
    }
    loopSleepMillis = interval;
}