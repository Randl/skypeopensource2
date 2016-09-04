cmake_minimum_required(VERSION 3.3)
project(skycontact4)

set(SOURCE_FILES blob_auth2.c blob_auth3.c blob_auth4.c blob_auth7.c blob_enc.c crc32_util.c dh_384.c for_dll.c get_blob.c miramax.c skycontact3.c skycontact4_dll.cpp skype/ext42.c skype/pack-4142.c skype/skype_rc4.c skype/unpack-4142.c skype_login.c sock_comm.c tcp_recv.c tcp_setup.c util.c)
set(HEADER_FILES decode41.h short_types.h skycontact4_dll.h skype/skype_basics.h skype/skype_rc4.h)

add_library(skycontact4 SHARED ${SOURCE_FILES})
target_link_libraries(skycontact4 crypto miracl)