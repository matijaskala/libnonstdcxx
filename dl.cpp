/*
 * Dynamic linking
 * Copyright (C) 2014  Matija Skala <mskala@gmx.com>
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "dl"

#ifdef _WIN32
#include <string>
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void* dl::open ( const char* file ) {
#ifdef _WIN32
    std::string mspath;
    for ( char* c = file; *c; c++ )
    	mspath += ( *c == '/' ) ? '\\' : *c;
    return LoadLibraryExA ( (LPSTR) mspath.c_str(), NULL, 
                           LOAD_WITH_ALTERED_SEARCH_PATH );
#else
    dlerror();
    return dlopen ( file, RTLD_NOW | RTLD_LOCAL );
#endif
}

bool dl::close ( void* handle ) {
#ifdef _WIN32
    return FreeLibrary ( static_cast<HMODULE> ( handle ) );
#else
    dlerror();
    return dlclose ( handle ) == 0;
#endif
}

void* dl::sym ( void* handle, const char* name ) {
#ifdef _WIN32
    FARPROC sym = GetProcAddress ( static_cast<HMODULE> ( handle ), name );
    return reinterpret_cast<void*> ( sym );
#else
    return dlsym ( handle, name );
#endif
}

char* dl::error () {
#ifdef _WIN32
    static char error_buffer[200];
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                   error_buffer, sizeof(error_buffer), NULL );
    return error_buffer;
#else
    return dlerror();
#endif
}
