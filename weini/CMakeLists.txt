cmake_minimum_required(VERSION 3.0)
project(weini CXX C)
set(CMAKE_BUILD_TYPE Release)


set (CMAKE_CXX_FLAGS "--std=c++11 -msse4.2 ${CMAKE_CXX_FLAGS} -O3 -g -mpopcnt ")
#-Wall -Wno-comment -Wno-unknown-pragmas -Wno-switch -Wno-reorder -Wno-char-subscripts")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA)

add_definitions("-DGIT_SHA=\"${GIT_SHA}\"")
add_definitions("-DGIT_VERSION=\"0.0.21\"")

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories("header/")
include_directories("dep/")
file(GLOB SOURCES "src/*.cpp")

add_executable(weini ${SOURCES})
set_property(TARGET weini PROPERTY INTERPROCEDURAL_OPTIMIZATION True)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(weini PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(weini "${CMAKE_THREAD_LIBS_INIT}")
endif()
