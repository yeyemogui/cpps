cmake_minimum_required(VERSION 3.20)
project(SortTest)
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
    add_definitions(-DDARWIN)
endif()
set(CMAKE_CXX_FLAGS "-g -O2")
add_executable(${PROJECT_NAME} main.cc
        ../Include/quick_sort.h
        Src/quick_sort_test.cc
        ../Include/quick_sort.h
        ../Include/quick_sort.h)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GTEST_LIBRARIES} GTest::gmock -latomic)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GTEST_LIBRARIES} GTest::gmock)
endif ()