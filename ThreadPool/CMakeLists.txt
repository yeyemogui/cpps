cmake_minimum_required(VERSION 3.20)
project(ThreadPoolTest)
message(STATUS "CMAKE_SYSTEM_NAME = '${CMAKE_SYSTEM_NAME}'")
set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
message(STATUS "GTEST_LIBRARIES = '${GTEST_LIBRARIES}'")
message(STATUS "GMOCK_LIBRARIES = '${GMOCK_LIBRARIES}'")
message(STATUS "CMAKE_SYSTEM_NAME = '${CMAKE_SYSTEM_NAME}'")
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
    include_directories(${GTEST_INCLUDE_DIRS})
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include_directories("/usr/local/include")
endif()
set(CMAKE_CXX_FLAGS "-g")
add_executable(${PROJECT_NAME} UT/main.cc
        UT/Src/thread_queue_safe_test.cc
        UT/Src/function_wrapper_test.cc
        UT/Src/threadPool_test.cpp
        Include/threadPool.h)

target_link_libraries(${PROJECT_NAME} PRIVATE ${GTEST_LIBRARIES} GTest::gmock)