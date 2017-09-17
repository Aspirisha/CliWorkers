find_package(Qt5Test REQUIRED)
find_package(Qt5Core REQUIRED)

set(CMAKE_AUTOMOC ON)
set(SOURCES main.cpp FractalWorkerTest.cpp)
set(HEADERS FractalWorkerTest.h)

add_executable(tests ${SOURCES} ${HEADERS})
qt5_use_modules(tests Core Test)
target_link_libraries(tests cliworkerslib)