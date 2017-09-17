#include <iostream>
#include "ConsoleReader.h"

void ConsoleReader::run() {
    for (;;) {
        std::string line;
        std::getline(std::cin, line);
        if (std::cin.eof() || line == "quit") {
            emit exitRequest();
            break;
        }
        emit gotLine(QString::fromStdString(line));
    }
}