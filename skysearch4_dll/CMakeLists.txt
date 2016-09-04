cmake_minimum_required(VERSION 3.3)
project(skysearch4)

set(SOURCE_FILES blob_snode1.c decode_profile.c decode_profile2.c dh_384.c for_dll.c get_blob.c global_ptr.c miramax.c skype/ext42.c skype/pack-4142.c skype/skype_rc4.c skype/unpack-4142.c skysearch3.c skysearch4_dll.cpp slots_util.c sock_comm.c sock_comm_udp.c tcp_setup.c udp_pkt.c util.c vcard_save.c)
set(HEADER_FILES short_types.h skype/skype_basics.h skype/skype_rc4.h skysearch4_dll.h slots_util.h stdafx.h targetver.h)

add_library(skysearch4 SHARED ${SOURCE_FILES})
target_link_libraries(skysearch4 crypto miracl)