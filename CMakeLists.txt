project(libnonstdc++)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)

include("${ECM_MODULE_DIR}/ECMUninstallTarget.cmake")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -pedantic")

add_library(char32 SHARED
    _c32toc8.c
    _c8len.c
    getc16.c
    getc32.c
    putc32.c
    utf8len.c
    utf8toutf16.c
    utf8toutf32.c
    ungetc32.c
)

add_library(nonstdc++ SHARED
    directory.cpp
    dl.cpp
    xterm.cpp
)

add_library(nonstdc++-extra SHARED
    cxxabi.cpp
    exceptions.cpp
    improved_istream.cpp
    tupleid.cpp
)

install(TARGETS char32 nonstdc++
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(FILES
    char32.h

    basic_variant
    buffered_ifstream
    casts
    cxxabi
    directory
    dl
    file
    funcid
    improved_istream
    signal
    stdio_sync_filebuf
    traits
    tupleid
    xterm
DESTINATION include/nonstd)
