cmake_minimum_required(VERSION 3.3)
project(skyrelay)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DSKYRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DSKYRELAY4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DSKYRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DSKYRELAY4_DLL_EXPORTS")
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSKYRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSKYRELAY4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSKYRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSKYRELAY4_DLL_EXPORTS")
endif ()

set(SOURCE_FILES ../util/crc32_util.c decode41.c dh384.c dh384_setup.c get_blob.c miramax.c relays_util.c skypush3.c skyrel3.c skyrelay3.c skyrelay4_dll.cpp sock_comm.c sock_comm_udp.c tcp_pkt.c tcp_recv.c udp_pkt.c util.c util_printf.c)
set(HEADER_FILES decode41.h defs.h relays_util.h skyrelay4_dll.h)

add_library(skyrelay_lib ${SOURCE_FILES})
target_link_libraries(skyrelay_lib crypto miracl skype)
if (WIN32)
    target_link_libraries(skyrelay_lib ws2_32)
endif ()

add_library(skyrelay SHARED for_dll.c)
target_link_libraries(skyrelay skyrelay_lib)

add_executable(skyrelay_exe for_cmd.c)
target_link_libraries(skyrelay_exe skyrelay_lib)