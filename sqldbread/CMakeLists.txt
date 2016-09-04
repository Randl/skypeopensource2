cmake_minimum_required(VERSION 3.3)
project(sqldbread)

set(SOURCE_FILES for_cmd.c for_dll.c sqllite_dbread.c sqllite_util.c unicode.c util.c util_printf.c db/sqlite3.c)
set(HEADER_FILES dbsql.h db/sqlite3.h sqlite3ext.h)

add_executable(sqldbread ${SOURCE_FILES})