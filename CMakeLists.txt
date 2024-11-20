cmake_minimum_required(VERSION 3.0)
project(my_project)

# Add the Raylib dependency
include_directories(/opt/homebrew/include)  # Adjust this path if necessary
link_directories(/opt/homebrew/lib)         # Adjust this path if necessary

add_executable(main main.c)
target_link_libraries(main raylib m pthread)

