cmake_minimum_required(VERSION 3.10)

project(libnonstdc++)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(ECMUninstallTarget)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.0 AND NOT NO_EXTRA)
        message(WARNING "GCC 5 is required to compile libnonstdc++-extra - disabling\nuse -DNO_EXTRA=ON to silence this warning")
        set(NO_EXTRA ON)
    endif(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.0 AND NOT NO_EXTRA)
endif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)

set(LIBNONSTDCXX_TARGETS char32 nonstdc++)
set(LIBNONSTDCXX_HEADERS
    char32.h

    dl
    lexical_cast
    traits
    type_array
    xterm
)

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
    dl.cpp
    xterm.cpp
)
target_compile_features(nonstdc++
    PUBLIC cxx_std_17)
if(NOT NO_EXTRA)
set(LIBNONSTDCXX_TARGETS ${LIBNONSTDCXX_TARGETS} nonstdc++-extra)
set(LIBNONSTDCXX_HEADERS ${LIBNONSTDCXX_HEADERS}
    basic_variant
    buffered_ifstream
    cxxabi
    power
)
add_library(nonstdc++-extra SHARED
    cxxabi.cpp
)
target_compile_features(nonstdc++-extra
    INTERFACE cxx_attributes cxx_inheriting_constructors cxx_variadic_templates)
endif(NOT NO_EXTRA)

install(TARGETS ${LIBNONSTDCXX_TARGETS}
    EXPORT LibNonStdC++Targets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(FILES ${LIBNONSTDCXX_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/nonstd")
install(FILES LibNonStdC++Config.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/LibNonStdC++")
install(EXPORT LibNonStdC++Targets DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/LibNonStdC++")
