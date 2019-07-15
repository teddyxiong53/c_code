cmake_minimum_required(VERSION 2.8)
add_definitions("-Wall  -DDEBUG=1 -g")
add_subdirectory(glib)

include_directories("./glib/include")

add_executable(test
			main.c
			)
target_link_libraries(test myglib)
