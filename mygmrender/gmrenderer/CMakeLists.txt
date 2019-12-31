cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig)

pkg_search_module( GSTREAMER REQUIRED gstreamer-1.0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ")
include_directories(${GSTREAMER_INCLUDE_DIRS})

add_executable(mygmrenderer
			src/main.c
)
target_link_libraries(mygmrenderer ${GSTREAMER_LIBRARIES})
