cmake_minimum_required(VERSION 3.25)
project(cutpack VERSION 1.0.2)

set(CMAKE_CXX_STANDARD 17)

add_executable(cutpack main.cpp CutpackOperation.h CutsceneFile.h QbCrc.cpp QbCrc.h)

target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)