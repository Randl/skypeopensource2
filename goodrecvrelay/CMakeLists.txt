cmake_minimum_required(VERSION 3.3)
project(goodrecvrelay)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DGOODRECVRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DGOODRECVRELAY4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DGOODRECVRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DGOODRECVRELAY4_DLL_EXPORTS")
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DGOODRECVRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DGOODRECVRELAY4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DGOODRECVRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DGOODRECVRELAY4_DLL_EXPORTS")
endif ()

set(SOURCE_FILES blob_newblk_recv.c chatrecv_newchat_init.c chatrecv_restorechat.c ../util/crc32_util.c cred_util.c dh_384.c get_blob.c get_blob2.c goodrecvrelay3.c headers_util.c miramax.c mycrypto.c recovery_signed_data.c relays_util.c relay_main.c restore_key.c skypush3.c skyrel3.c skyrelay2.c sock_comm.c sock_comm_udp.c sqllite_recv.c sqllite_util.c tcp_dbio.c tcp_fileio.c tcp_newblk_prep.c tcp_pkt.c tcp_recv.c tcp_sess1.c tcp_sess1_old.c tcp_setup.c tools_file.c udp_pkt.c unicode.c util.c util_printf.c)
set(HEADER_FILES dbsql.h defs.h goodrecvrelay4_dll.h headers_util.h relays_util.h)

add_library(goodrecvrelay_lib ${SOURCE_FILES})
target_link_libraries(goodrecvrelay_lib sqlite crypto miracl skype relay_blob)
if (WIN32)
    target_link_libraries(goodrecvrelay_lib ws2_32)
endif ()


add_library(goodrecvrelay SHARED for_dll.c)
target_link_libraries(goodrecvrelay goodrecvrelay_lib)

add_executable(goodrecvrelay_exe for_cmd.c)
target_link_libraries(goodrecvrelay_exe goodrecvrelay_lib)
