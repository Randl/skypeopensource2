cmake_minimum_required(VERSION 3.3)
project(sqlite3)

set(SOURCE_FILES sqlite3.c)
set(HEADER_FILES sqlite.h sqlite3ext.h)

add_library(sqlite3 ${SOURCE_FILES})