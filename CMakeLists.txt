cmake_minimum_required(VERSION 3.1)
project(Autofocus)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(OpenCV 4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(Autofocus Autofocus.cpp)
target_link_libraries(Autofocus ${OpenCV_LIBS})
