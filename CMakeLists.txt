project(libnonstdc++)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_DIR})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(ECMUninstallTarget)

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
target_compile_features(nonstdc++
    INTERFACE cxx_return_type_deduction)

add_library(nonstdc++-extra SHARED
    cxxabi.cpp
    improved_istream.cpp
)
target_compile_features(nonstdc++-extra
    INTERFACE cxx_attributes cxx_nullptr cxx_variadic_templates)

install(FILES
    char32.h

    basic_variant
    buffered_ifstream
    cxxabi
    directory
    dl
    file
    funcid
    improved_istream
    lexical_cast
    nullref
    runtime_cast
    signal
    stdio_sync_filebuf
    traits
    tupleid
    xterm
DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/nonstd")

install(TARGETS
    char32
    nonstdc++
    nonstdc++-extra
EXPORT LibNonStdC++Targets
RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(FILES LibNonStdC++Config.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/LibNonStdC++")
install(EXPORT LibNonStdC++Targets DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/LibNonStdC++")
