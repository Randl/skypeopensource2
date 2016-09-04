cmake_minimum_required(VERSION 3.3)
project(skycontact3)

set(SOURCE_FILES blob_auth2.c blob_auth3.c blob_auth4.c blob_auth7.c blob_enc.c crc32_util.c dh_384.c for_cmd.c get_blob.c miramax.c skycontact3.c skype_login.c sock_comm.c tcp_recv.c tcp_setup.c util.c)
set(HEADER_FILES decode41.h short_types.h)

add_executable(skycontact3 ${SOURCE_FILES})
target_link_libraries(skycontact3 crypto miracl skype)
if (WIN32)
    target_link_libraries(skycontact3 ws2_32)
endif ()