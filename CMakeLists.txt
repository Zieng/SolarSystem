cmake_minimum_required(VERSION 3.3)
project(SolarSystem)

#set the working directory to ./build
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -v")

set(SOURCE_FILES main.cpp Planet.cpp Planet.h)

add_executable(SolarSystem ${SOURCE_FILES})

# include folder path
include_directories(/usr/local/include)

#link_libraries(/usr/local/lib)

#find frameworks need and set them to variable 'frameworks'
find_library(iokit_lib IOKit)
find_library(cocoa_lib Cocoa)
find_library(opengl_lib OpenGL)
find_library(corefoundation_lib CoreFoundation)
find_library(glut_lib GLUT)
find_library(coreaudio_lib CoreAudio)
find_library(corevideo_lib CoreVideo)
find_library(soil_lib SOIL)
find_library(glew_lib glew)
find_library(glfw3_lib glfw3)
set(frameworks
        ${iokit_lib}
        ${cocoa_lib}
        ${opengl_lib}
        ${corefoundation_lib}
        ${glut_lib}
        ${corevideo_lib}
        ${coreaudio_lib}
        ${soil_lib}
        ${glew_lib}
        ${glfw3_lib}
)

target_link_libraries(SolarSystem ${frameworks})



