cmake_minimum_required(VERSION 3.3)
project(skysearch4)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DSKYSEARCH4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DSKYSEARCH4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DSKYSEARCH4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DSKYSEARCH4_DLL_EXPORTS")
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSKYSEARCH4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSKYSEARCH4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSKYSEARCH4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSKYSEARCH4_DLL_EXPORTS")
endif ()

set(SOURCE_FILES blob_snode1.c decode_profile.c decode_profile2.c dh_384.c for_dll.c get_blob.c global_ptr.c miramax.c skysearch3.c skysearch4_dll.cpp slots_util.c sock_comm.c sock_comm_udp.c tcp_setup.c udp_pkt.c util.c util_printf.c vcard_save.c)
set(HEADER_FILES short_types.h skysearch4_dll.h slots_util.h stdafx.h targetver.h)

add_library(skysearch4 SHARED ${SOURCE_FILES})
target_link_libraries(skysearch4 crypto miracl skype)
if (WIN32)
    target_link_libraries(skysearch4 ws2_32)
endif ()