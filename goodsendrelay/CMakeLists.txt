cmake_minimum_required(VERSION 3.3)
project(goodsendrelay)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DGOODSENDRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DGOODSENDRELAY4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DGOODSENDRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DGOODSENDRELAY4_DLL_EXPORTS")
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DGOODSENDRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DGOODSENDRELAY4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DGOODSENDRELAY4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DGOODSENDRELAY4_DLL_EXPORTS")
endif ()

set(SOURCE_FILES chatsend_newchat_init.c chatsend_restorechat.c chatsend_restorechat2.c crc32_util.c cred_util.c dh_384.c for_cmd.c for_dll.c get_blob.c get_blob2.c get_blob3.c goodsendrelay3.c headers_util.c miramax.c mycrypto.c recovery_signed_data.c relays_util.c relay_main.c restore_key.c skypush3.c skyrel3.c skyrelay2.c sock_comm.c sock_comm_udp.c sqllite_send.c sqllite_util.c tcp_dbio.c tcp_fileio.c tcp_newblk_prep.c tcp_pkt.c tcp_recv.c tcp_sess1.c tcp_setup.c tools_file.c udp_pkt.c unicode.c util.c util_printf.c)
set(HEADER_FILES dbsql.h defs.h goodsendrelay4_dll.h headers_util.h relays_util.h short_types.h)

add_library(goodsendrelay SHARED ${SOURCE_FILES})
target_link_libraries(goodsendrelay sqlite crypto miracl skype relay_blob)
if (WIN32)
    target_link_libraries(goodsendrelay ws2_32)
endif ()

add_executable(goodsendrelay_exe ${SOURCE_FILES})
target_link_libraries(goodsendrelay_exe sqlite crypto miracl skype relay_blob)
if (WIN32)
    target_link_libraries(goodsendrelay_exe ws2_32)
endif ()