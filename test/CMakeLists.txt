find_package(Qt5Test REQUIRED)
find_package(Qt5Core REQUIRED)

set(CMAKE_AUTOMOC ON)
set(SOURCES main.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(tests Qt5::Core Qt5::Test)