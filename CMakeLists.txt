cmake_minimum_required(VERSION 3.1)
project(Autofocus)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")



find_package(OpenCV 4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(main ${SOURCES})
target_link_libraries(main ${OpenCV_LIBS})
