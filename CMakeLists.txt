cmake_minimum_required(VERSION 3.20)
project(demo)
message(STATUS "CMAKE_SYSTEM_NAME = '${CMAKE_SYSTEM_NAME}'")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_CXX_COMPILER "/usr/bin/g++")
	set(CMAKE_CXX_FLAGS "-g")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	add_definitions(-DDARWIN)
endif()

add_executable(${PROJECT_NAME} main.cc
		ThreadPool/Include/threadPool.h
		ThreadPool/Include/Functor.h
		ThreadPool/Include/thread_queue_safe.h
		ThreadPool/Include/function_wrapper.h
		ThreadPool/Include/thread_queue_base.h
		ThreadPool/Include/thread_queue_factory.h)
