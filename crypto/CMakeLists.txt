cmake_minimum_required(VERSION 3.3)
project(crypto)

set(SOURCE_FILES md5.c rijndael.c sha1.c)
set(HEADER_FILES md5.h rijndael.h sha1.h)

add_library(crypto ${SOURCE_FILES})