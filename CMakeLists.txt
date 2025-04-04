cmake_minimum_required(VERSION 3.25)
project(cutpack VERSION 1.0.2)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        src/main.cpp
        src/util/qb_crc.cpp
)

add_executable(cutpack ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)
target_include_directories(${PROJECT_NAME} PRIVATE include)