cmake_minimum_required(VERSION 3.10)

project(carrerouge C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(carrerouge
    main.c
)

target_include_directories(carrerouge PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(carrerouge
    ${SDL2_LIBRARIES}
)
