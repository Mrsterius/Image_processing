project(ConstructWall)
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

find_package(OpenCV REQUIRED)

# Project Executable
add_executable ( ../bin/RefiningContours ../src/Image\ Processing/RefiningContours.cpp)
target_link_libraries(../bin/RefiningContours ${OpenCV_LIBS})
