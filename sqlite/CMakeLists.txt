cmake_minimum_required(VERSION 3.3)
project(sqlite)

set(SOURCE_FILES sqlite3.c)
set(HEADER_FILES sqlite.h sqlite3ext.h)

add_library(sqlite ${SOURCE_FILES})