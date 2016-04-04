
cmake_minimum_required (VERSION 2.8)
# Project 
project (RouterSvr)


# policy
cmake_policy(SET CMP0015 OLD)

message (STATUS "compiler is ${CMAKE_CXX_COMPILER_ID}" )
message (STATUS "source dir: ${PROJECT_SOURCE_DIR} ")
message (STATUS "current dir: ${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT win32)
    add_definitions(-std=c++11 -g)
endif()



#  设置protobuf 生成文件的路径
set (Pb_gen_root_path	../protobuf_gen)
set (Pb_gen_file_path	${Pb_gen_root_path}/include/)

include_directories(${Pb_gen_file_path})


#  设置protobuf文件路径
set (Pb_head_path 	../protobuf/)
set (Pb_lib_path	../protobuf/lib)

include_directories(${Pb_head_path})
LINK_DIRECTORIES(${Pb_lib_path})



# find Boost
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.50.0 REQUIRED COMPONENTS system serialization)
if(Boost_FOUND)

    message(STATUS  "Boost include dirs: " ${Boost_INCLUDE_DIRS})
    message(STATUS  "Boost lib dirs: " ${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
endif()


#find protobuf
find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
    message(STATUS "protobuf include dirs: " ${PROTOBUF_INCLUDE_DIRS})
    message(STATUS "protobuf lib dirs: " ${PROTOBUF_LIBRARY})

    include_directories(${PROTOBUF_INCLUDE_DIRS})
endif()



aux_source_directory (. DIR_SRCS)
aux_source_directory (${Pb_gen_file_path} PB_GEN_SRCS)

file(GLOB_RECURSE HEAD_FILES *h *hpp)
file(GLOB_RECURSE PB_GEN_HEAD_FILES ../protobuf_gen/include/*.h ../protobuf_gen/include/*.hpp)


ADD_EXECUTABLE(${PROJECT_NAME} ${DIR_SRCS} ${PB_GEN_SRCS} ${HEAD_FILES} ${PB_GEN_HEAD_FILES})


TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROTOBUF_LIBRARIES} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} -lpthread)






