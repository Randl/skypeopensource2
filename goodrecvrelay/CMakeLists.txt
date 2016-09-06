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

set(SOURCE_FILES blobs/blob1.c blobs/blob2.c blobs/blob53.c blobs/blob58.c blobs/blob6Dreq.c blobs/blob6D_cmd0C_chatend.c blobs/blob6D_cmd0D_chatinit.c blobs/blob6D_cmd10_unkheader.c blobs/blob6D_cmd13_sendend.c blobs/blob6D_cmd13_sendheaders.c blobs/blob6D_cmd13_sendheaders2.c blobs/blob6D_cmd15_reqmsgbody.c blobs/blob6D_cmd1B_closeconnect.c blobs/blob6D_cmd1D_requic.c blobs/blob6D_cmd24_chatsign.c blobs/blob6D_cmd27_finish.c blobs/blob6D_cmd2A_headerssign.c blobs/blob6D_cmd2B_msgbody.c blobs/blob7A.c blobs/blob7B.c blobs/blob7D.c blobs/blob_enc.c blobs/blob_newblk.c blobs/recv_blob6D_cmd0Fsmall_chatok.c blobs/recv_blob6D_cmd0F_chatok.c blobs/recv_blob6D_cmd10_lastmyheader.c blobs/recv_blob6D_cmd10_pos1.c blobs/recv_blob6D_cmd10_pos2.c blobs/recv_blob6D_cmd13_sendheaders.c blobs/recv_blob6D_cmd13_sendheaders2.c blobs/recv_blob6D_cmd13_sendheaders3.c blobs/recv_blob6D_cmd13_sendheaders4.c blobs/recv_blob6D_cmd13_sendheaders5.c blobs/recv_blob6D_cmd15_reqmsgbody.c blobs/recv_blob6D_cmd1E_uicreply.c blobs/recv_blob6D_cmd23_initreq.c blobs/recv_blob6D_cmd27_finish.c blobs/recv_blob6D_cmd29_sign2req.c blobs/recv_blob6D_cmd2B_msgbody.c blobs/recv_blob6D_cmd46_chatend.c blobs/recv_blob6D_error.c blobs/recv_blob6D_error2.c chatrecv_newchat_init.c chatrecv_restorechat.c crc32_util.c cred_util.c dh_384.c for_cmd.c for_dll.c get_blob.c get_blob2.c goodrecvrelay3.c headers_util.c miramax.c mycrypto.c recovery_signed_data.c relays_util.c relay_main.c restore_key.c skypush3.c skyrel3.c skyrelay2.c sock_comm.c sock_comm_udp.c sqllite_recv.c sqllite_util.c tcp_dbio.c tcp_fileio.c tcp_newblk_prep.c tcp_pkt.c tcp_recv.c tcp_sess1.c tcp_sess1_old.c tcp_setup.c tools_file.c udp_pkt.c unicode.c util.c util_printf.c)
set(HEADER_FILES dbsql.h decode41.h defs.h goodrecvrelay4_dll.h headers_util.h relays_util.h short_types.h)

add_library(goodrecvrelay SHARED ${SOURCE_FILES})
target_link_libraries(goodrecvrelay sqlite crypto miracl skype)
if (WIN32)
    target_link_libraries(goodrecvrelay ws2_32)
endif ()


add_executable(goodrecvrelay_exe ${SOURCE_FILES})
target_link_libraries(goodrecvrelay_exe sqlite crypto miracl skype)
if (WIN32)
    target_link_libraries(goodrecvrelay_exe ws2_32)
endif ()
