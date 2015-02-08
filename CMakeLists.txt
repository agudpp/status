project(status)
cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)

add_definitions(-DDEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(SRC_LIST ${SRC_LIST}
    ./StatusHandler.h
    ./Debug.h
    ./TimeHelper.h
    ./SampleModule.h
    ./SampleModule.cpp
)


add_executable(${PROJECT_NAME} ${SRC_LIST})

