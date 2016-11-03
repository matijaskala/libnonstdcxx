project(libnonstdc++)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)

include(GNUInstallDirs)
include("${ECM_MODULE_DIR}/ECMUninstallTarget.cmake")

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
    PUBLIC cxx_return_type_deduction)

add_library(nonstdc++-extra SHARED
    cxxabi.cpp
    improved_istream.cpp
    tupleid.cpp
)
target_compile_features(nonstdc++-extra
    PUBLIC cxx_attributes cxx_nullptr cxx_variadic_templates
    PRIVATE cxx_auto_type cxx_range_for)

install(TARGETS char32 nonstdc++ nonstdc++-extra EXPORT LibNonStdC++Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
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
DESTINATION include/nonstd)

install(FILES LibNonStdC++Config.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/LibNonStdC++")
install(EXPORT LibNonStdC++Targets DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/LibNonStdC++")
