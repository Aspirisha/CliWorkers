#include <QTextStream>
#include "Cli.h"

static constexpr int maxThreads = 100;

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("CliWorkers");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption threads("threads", QString("Number of threads to run (from 1 to %1)").arg(maxThreads), 
        QCoreApplication::translate("main", "integer"));
    parser.addOption(threads);
    parser.process(app);
    bool ok;
    int threadsNum = parser.value(threads).toInt(&ok);

    if (!ok || threadsNum < 1 || threadsNum > maxThreads) {
        QTextStream out(stdout);
        out << "Invalid number of threads: " << parser.value(threads) << endl;
        return 0;
    }

    Cli *cli = new Cli(threadsNum, &app);
    QObject::connect(cli, SIGNAL(finished()), &app, SLOT(quit()));
    QTimer::singleShot(0, cli, SLOT(run()));
    return app.exec();
}