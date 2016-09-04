cmake_minimum_required(VERSION 3.3)
project(skyauth3)

set(SOURCE_FILES blob_enc.c blob_login.c dh_384.c dump.c miramax.c skyauth3.c skype_login.c sock_comm.c tcp_setup.c util.c util_printf.c)
set(HEADER_FILES decode41.h short_types.h)

add_executable(skyauth3 ${SOURCE_FILES})
target_link_libraries(skyauth3 crypto miracl skype)
if (WIN32)
    target_link_libraries(skyauth3 ws2_32)
endif ()