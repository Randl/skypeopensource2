cmake_minimum_required(VERSION 3.3)
project(skycontact4)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DSKYCONTACT4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DSKYCONTACT4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DSKYCONTACT4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DSKYCONTACT4_DLL_EXPORTS")
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSKYCONTACT4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSKYCONTACT4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSKYCONTACT4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSKYCONTACT4_DLL_EXPORTS")
endif ()

set(SOURCE_FILES blob_auth2.c blob_auth3.c blob_auth4.c blob_auth7.c blob_enc.c crc32_util.c dh_384.c for_dll.c get_blob.c miramax.c skycontact3.c skycontact4_dll.cpp skype_login.c sock_comm.c tcp_recv.c tcp_setup.c util.c util_printf.c)
set(HEADER_FILES decode41.h short_types.h skycontact4_dll.h)

add_library(skycontact4 SHARED ${SOURCE_FILES})
target_link_libraries(skycontact4 crypto miracl skype)
if (WIN32)
    target_link_libraries(skycontact4 ws2_32)
endif ()