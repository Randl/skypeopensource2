cmake_minimum_required(VERSION 3.3)
project(skyrelay4)

set(SOURCE_FILES crc32_util.c decode41.c dh384.c dh384_setup.c for_dll.c get_blob.c miramax.c relays_util.c skypush3.c skyrel3.c skyrelay3.c skyrelay4_dll.cpp sock_comm.c sock_comm_udp.c tcp_pkt.c tcp_recv.c udp_pkt.c util.c)
set(HEADER_FILES decode41.h defs.h relays_util.h short_types.h skyrelay4_dll.h)

add_library(skyrelay4 SHARED ${SOURCE_FILES})
target_link_libraries(skyrelay4 crypto miracl skype)
if (WIN32)
    target_link_libraries(skyrelay4 ws2_32)
endif ()