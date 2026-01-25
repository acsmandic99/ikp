cmake_minimum_required(VERSION 3.10)
project(ikp_projekat C)

set(CMAKE_C_STANDARD 99)
find_package(Threads REQUIRED)
set(CMAKE_BUILD_TYPE Debug)

include_directories(
    .
    agregator
    agregator/thread-ovi/parent_thread
    agregator/thread-ovi/listener_thread
    agregator/thread-ovi/worker_thread
    strukture/hashmapa
    strukture/thread_pool
    messages
    data_destination
)


set(AGGR_MAIN "main.c")

# Uzimamo sve .c fajlove iz agregator i strukture foldera
file(GLOB_RECURSE AGGR_CORE "agregator/*.c")
file(GLOB_RECURSE STRUKTURE_CORE "strukture/*.c")

# Spajamo ih, ali NIŠTA iz data_destination ne ulazi ovde
set(AGGR_SOURCES ${AGGR_MAIN} ${AGGR_CORE} ${STRUKTURE_CORE})

add_executable(agregator_app ${AGGR_SOURCES})
# Koristi ugrađenu varijablu koju generiše find_package(Threads)
target_link_libraries(agregator_app PRIVATE Threads::Threads m)

set(PARENT_SOURCES 
    "data_destination/main.c" 
    "data_destination/data_destination.c"
)

add_executable(parent_app ${PARENT_SOURCES})
target_link_libraries(parent_app PRIVATE Threads::Threads m)
