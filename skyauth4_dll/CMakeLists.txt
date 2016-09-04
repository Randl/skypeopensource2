cmake_minimum_required(VERSION 3.3)
project(skyauth4)

set(SOURCE_FILES blob_enc.c blob_login.c crypto/md5.c crypto/rijndael.c crypto/sha1.c dh_384.c dllmain.cpp)
set(HEADER_FILES crypto/md5.h crypto/rijndael.h crypto/sha1.h decode41.h miracl_lib/miracl.h miracl_lib/mirdef.h short_types.h skyauth4_dll.h skype/skype_basics.h skype/skype_rc4.h stdafx.h targetver.h)

add_library(skyauth4 SHARED ${SOURCE_FILES})