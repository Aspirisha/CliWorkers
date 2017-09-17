#include <iostream>
#include "Cli.h"

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    Cli *cli = new Cli(&app);
    QObject::connect(cli, SIGNAL(finished()), &app, SLOT(quit()));
    QTimer::singleShot(0, cli, SLOT(run()));
    return app.exec();
}