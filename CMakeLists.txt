cmake_minimum_required(VERSION 3.22)
project(ex4 C)

set(CMAKE_C_STANDARD 99)

add_executable(ex4 ex4_srv.c ex4_client.c)
