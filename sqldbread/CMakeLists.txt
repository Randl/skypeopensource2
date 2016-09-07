cmake_minimum_required(VERSION 3.3)
project(sqldbread)

set(SOURCE_FILES sqllite_dbread.c sqllite_util.c unicode.c util.c util_printf.c)
set(HEADER_FILES dbsql.h)

add_library(sqldbread_lib ${SOURCE_FILES})
target_link_libraries(sqldbread_lib sqlite)


add_library(sqldbread SHARED for_dll.c)
target_link_libraries(sqldbread sqldbread_lib)

add_library(sqldbread_exe for_cmd.c)
target_link_libraries(sqldbread_exe sqldbread_lib)