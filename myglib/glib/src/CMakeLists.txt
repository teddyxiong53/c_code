cmake_minimum_required(VERSION 2.8)

include_directories("../include")
add_library(myglib STATIC
    gmain.c
    glib-init.c
    gmessages.c
    gtestutils.c
)