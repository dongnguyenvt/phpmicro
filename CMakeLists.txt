cmake_minimum_required(VERSION 3.24)
project(phpmicro C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(phpmicro
        php_micro.c
        php_micro.h
        php_micro_fileinfo.c
        php_micro_fileinfo.h
        php_micro_helper.c
        php_micro_helper.h
        php_micro_hooks.c
        php_micro_hooks.h)
