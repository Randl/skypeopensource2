cmake_minimum_required(VERSION 3.3)
project(skyauth3)

set(SOURCE_FILES blob_enc.c blob_login.c dh_384.c dump.c miramax.c skyauth3.c skype/ext42.c skype/pack-4142.c skype/skype_rc4.c skype/unpack-4142.c skype_login.c sock_comm.c tcp_setup.c util.c)
set(HEADER_FILES decode41.h short_types.h skype/skype_basics.h skype/skype_rc4.h)

add_executable(skyauth3 ${SOURCE_FILES})
target_link_libraries(skyauth3 crypto miracl)