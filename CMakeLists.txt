cmake_minimum_required(VERSION 3.3)
project(ls-repl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES
    main.c)

add_executable(ls-repl ${SOURCE_FILES})
