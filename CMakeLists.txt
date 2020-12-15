cmake_minimum_required(VERSION 3.10.2)
project(Block3 C)

set(CMAKE_C_STANDARD 99)

add_executable(server server.c  Protokoll.c Protokoll.h uthash.h )
add_executable(client client.c Protokoll.c Protokoll.h)