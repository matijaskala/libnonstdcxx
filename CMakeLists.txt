project(libnonstdc++)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)

include("${ECM_MODULE_DIR}/ECMUninstallTarget.cmake")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -pedantic")

add_library(nonstdc++ SHARED
    cxxabi.cpp
    directory.cpp
    dl.cpp
    exceptions.cpp
    stdio_sync_filebuf.cpp
    tupleid.cpp
    xterm.cpp
)

install(TARGETS nonstdc++
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(FILES
    basic_variant
    buffered_ifstream
    casts
    cxxabi
    directory
    dl
    file
    funcid
    signal
    stdio_sync_filebuf
    traits
    tupleid
    xterm
DESTINATION include/nonstd)
