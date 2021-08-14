cmake_minimum_required(VERSION 3.10)

project(compute-shaders C)
set(CMAKE_C_STANDARD 11)
set(CMAKE)
add_compile_options(-Wall -Wextra -pedantic)

include_directories(./util/)
add_executable(main main.c)
target_link_libraries(main GL GLEW glfw) 