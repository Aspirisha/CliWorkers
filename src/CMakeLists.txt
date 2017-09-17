set(SOURCES Worker.cpp Cli.cpp WorkerManager.cpp QuotesWorker.cpp ConsoleReader.cpp FractalWorker.cpp)
set(HEADERS Worker.h Cli.h WorkerManager.h QuotesWorker.h ConsoleReader.h FractalWorker.h)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Xml REQUIRED)

add_library(cliworkerslib ${SOURCES} ${HEADERS})
qt5_use_modules(cliworkerslib Core Network Xml)

add_executable(cliworkers main.cpp)
target_link_libraries(cliworkers cliworkerslib)