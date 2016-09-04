cmake_minimum_required(VERSION 3.3)
project(skyauth4)

set(SOURCE_FILES blob_enc.c blob_login.c dh_384.c dllmain.cpp util.c util_printf.c)
set(HEADER_FILES decode41.h short_types.h skyauth4_dll.h stdafx.h targetver.h)

add_library(skyauth4 SHARED ${SOURCE_FILES})
target_link_libraries(skyauth4 crypto miracl skype)
if (WIN32)
    target_link_libraries(skyauth4 ws2_32)
endif ()