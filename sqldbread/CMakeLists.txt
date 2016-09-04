cmake_minimum_required(VERSION 3.3)
project(sqldbread)

set(SOURCE_FILES for_cmd.c for_dll.c sqllite_dbread.c sqllite_util.c unicode.c util.c util_printf.c)
set(HEADER_FILES dbsql.h)

add_executable(sqldbread ${SOURCE_FILES})
target_link_libraries(sqldbread sqlite)