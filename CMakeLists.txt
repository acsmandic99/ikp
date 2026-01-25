cmake_minimum_required(VERSION 3.10)
project(ikp_projekat C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


include_directories(
    .
    agregator
    strukture/hashmapa
    strukture/lista
    strukture/thread_pool
)


file(GLOB_RECURSE SOURCES 
    "main.c"
    "agregator/*.c"
    "strukture/*.c"
)


add_executable(server_app ${SOURCES})


target_link_libraries(server_app PRIVATE Threads::Threads m)
