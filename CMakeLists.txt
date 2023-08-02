cmake_minimum_required(VERSION 3.25)
project(cutpack)

set(CMAKE_CXX_STANDARD 17)

add_executable(cutpack main.cpp CutpackOperation.h CutsceneFile.h QbCrc.cpp QbCrc.h)
