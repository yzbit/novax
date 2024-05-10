cmake_minimum_required(VERSION 3.18)
project(sqlite3 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g -Wall")
set(CMAKE_C_FLAGS "${CMAKE_CX_FLAGS}  -g -Wall")

add_library(${PROJECT_NAME} SHARED sqlite3.c) 

#pthread dl
#install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (TARGETS ${PROJECT_NAME} DESTINATION .)