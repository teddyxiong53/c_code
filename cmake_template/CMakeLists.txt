cmake_minimum_required(VERSION 2.8)
add_definitions("-Wall -std=c++11 -DDEBUG=1")
add_subdirectory(mod1)
add_subdirectory(mod2)
include_directories("./mod1/include")
include_directories("./mod2/include")
add_executable(xxx 
			main.cpp
			)
target_link_libraries(xxx mod1 mod2)
