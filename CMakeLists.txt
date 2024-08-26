cmake_minimum_required(VERSION 3.18)
project(reactor VERSION 1.0.0 LANGUAGES CXX)

#add_compile_options(--coverage)
#add_link_options(--coverage)

#add_subdirectory(lib/sqlite)
add_subdirectory(src/core)
add_subdirectory(src/app)
add_subdirectory(src/test)
