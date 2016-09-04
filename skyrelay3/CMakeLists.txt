cmake_minimum_required(VERSION 3.3)
project(skyrelay3)

set(SOURCE_FILES crc32_util.c decode41.c dh384.c dh384_setup.c for_cmd.c get_blob.c miramax.c relays_util.c skype/ext42.c skype/pack-4142.c skype/skype_rc4.c skype/unpack-4142.c skypush3.c skyrel3.c skyrelay3.c sock_comm.c sock_comm_udp.c tcp_pkt.c tcp_recv.c udp_pkt.c util.c)
set(HEADER_FILES decode41.h defs.h relays_util.h short_types.h skype/skype_basics.h skype/skype_rc4.h)

add_executable(skyrelay3 ${SOURCE_FILES})
target_link_libraries(skyrelay3 crypto miracl)