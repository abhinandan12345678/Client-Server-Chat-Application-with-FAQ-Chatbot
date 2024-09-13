cmake_minimum_required(VERSION 3.27)
project(FAQ_D1 C)

set(CMAKE_C_STANDARD 11)

add_executable(FAQ_D1 server.c
        uuid4.h
        uuid4.c
        utils.c
        utils.h)
add_executable(FAQ_D1_C client.c
        uuid4.h
        uuid4.c
        utils.c
        utils.h)
