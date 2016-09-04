cmake_minimum_required(VERSION 3.3)
project(skype)

set(SOURCE_FILES ext42.c ext42_2.c ext42_checkblob.c ext42_getobj00.c ext42_getobj00seq.c ext42_getobj02ip.c pack-4142.c skype_rc4.c unpack-4142.c)
set(HEADER_FILES skype_basics.h skype_rc4.h)

add_library(skype ${SOURCE_FILES})