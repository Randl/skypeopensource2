cmake_minimum_required(VERSION 3.3)
project(skypeopensource2)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/w /Ox")
    set(CMAKE_C_FLAGS_RELEASE "/w /Ox")

    set(CMAKE_CXX_FLAGS_DEBUG "/Wall /Od /DEBUG")
    set(CMAKE_C_FLAGS_DEBUG "/Wall /Od /DEBUG")
    #TODO: flags
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fstrict-aliasing -flto -fno-fat-lto-objects")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer -march=native -w -save-temps -v  -da -Q")

    set(CMAKE_C_FLAGS_RELEASE "-std=c11")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -fstrict-aliasing -flto -fno-fat-lto-objects")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fomit-frame-pointer -march=native -w  -save-temps -v -da -Q")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-O1 -flto -s")

    set(CMAKE_CXX_FLAGS_DEBUG "-g -pg -std=c++11 -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wshadow -Wstrict-aliasing -Weffc++")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-inline-small-functions -fno-inline")
    set(CMAKE_C_FLAGS_DEBUG "-g -pg -std=c11 -O0 -DDEBUG")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Wshadow -Wstrict-aliasing")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-inline-small-functions -fno-inline")
endif ()
include_directories("include")

add_subdirectory(sqlite)
add_subdirectory(crypto)
add_subdirectory(miracl)
add_subdirectory(skype)
add_subdirectory(relay_blob)


add_subdirectory(goodrecvrelay)
add_subdirectory(goodsendrelay)
add_subdirectory(skyauth)
add_subdirectory(skycontact)
add_subdirectory(skyrelay)
add_subdirectory(skysearch)
add_subdirectory(sqldbread)