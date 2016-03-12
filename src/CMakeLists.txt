
cmake_minimum_required (VERSION 2.8)
# Project 
project (RouterSvr)


# policy
cmake_policy(SET CMP0015 OLD)

message (STATUS "compiler is ${CMAKE_CXX_COMPILER_ID}" )
message (STATUS "source dir: ${PROJECT_SOURCE_DIR} ")
message (STATUS "current dir: ${CMAKE_CURRENT_SOURCE_DIR}")

ADD_DEFINITIONS (-std=c++0x -g -Wall )

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
    elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

    else ()
        message(FATAL_ERROR "Your C++ compiler does not support C++11.")
    endif ()



# find Boost
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.50.0 REQUIRED COMPONENTS system serialization)
if(Boost_FOUND)

    message(STATUS  "Boost include dirs: " ${Boost_INCLUDE_DIRS})
    message(STATUS  "Boost lib dirs: " ${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
endif()


# 当前目前下的所有源文件
aux_source_directory (. DIR_SRCS)
ADD_EXECUTABLE(${PROJECT_NAME} ${DIR_SRCS})


#
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} -lpthread)


