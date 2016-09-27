cmake_minimum_required(VERSION 3.3)
project(skyauth)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DSKYAUTH4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DSKYAUTH4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DSKYAUTH4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DSKYAUTH4_DLL_EXPORTS")
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSKYAUTH4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSKYAUTH4_DLL_EXPORTS")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSKYAUTH4_DLL_EXPORTS")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSKYAUTH4_DLL_EXPORTS")
endif ()

set(SOURCE_FILES blob_enc.c blob_login.c dump.c dh_384.c dllmain.cpp skyauth4.c miramax.c skype_login.c sock_comm.c stdafx.cpp tcp_setup.c skyauth4_dll.cpp util.c util_printf.c)
set(HEADER_FILES decode41.h skyauth4_dll.h stdafx.h targetver.h)

add_library(skyauth_lib ${SOURCE_FILES})
target_link_libraries(skyauth_lib crypto miracl skype)
if (WIN32)
    target_link_libraries(skyauth_lib ws2_32)
endif ()

add_library(skyauth SHARED for_dll.c)
target_link_libraries(skyauth skyauth_lib)

add_executable(skyauth_exe skyauth3.c)
target_link_libraries(skyauth_exe skyauth_lib)