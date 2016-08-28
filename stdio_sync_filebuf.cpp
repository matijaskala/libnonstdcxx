/*
 * Iostreams wrapper for stdio FILE
 * Copyright (C) 2016  Matija Skala <mskala@gmx.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "stdio_sync_filebuf"

using namespace non_std;

static void c32toc8 ( std::char_traits<char32_t>::int_type c32, char* c8 ) {
    int len;
    if ( c32 < 0x80 )
        len = 1;
    else if ( c32 < 0x800 )
        len = 2;
    else if ( c32 < 0x10000 )
        len = 3;
    else if ( c32 < 0x200000 )
        len = 4;
    else if ( c32 < 0x4000000 )
        len = 5;
    else
        len = 6;
    c8[len] = 0;
    if (len == 1) {
        *c8 = c32;
        return;
    }
    for ( int i = len - 1; i >= 0; i-- ) {
        c8[i] = 0x80 | (c32 & 0x3f);
        c32 >>= 6;
    }
    c8[0] |= 0xff << (8 - len);
}

  template<>
    stdio_sync_filebuf<char32_t>::int_type
    stdio_sync_filebuf<char32_t>::syncgetc()
    {
        int len;
        int_type __c = getc(file());
        if ( __c == (int_type)EOF )
            return traits_type::eof();
        if ( __c < 0x80 )
            return __c;
        else if ( __c < 0xc0 )
            return -1;
        else if ( __c < 0xe0 ) {
            len = 2;
            __c &= 0x1f;
        }
        else if ( __c < 0xf0 ) {
            len = 3;
            __c &= 0x0f;
        }
        else if ( __c < 0xf8 ) {
            len = 4;
            __c &= 0x07;
        }
        else if ( __c < 0xfc ) {
            len = 5;
            __c &= 0x03;
        }
        else if ( __c < 0xfe ) {
            len = 6;
            __c &= 0x01;
        }
        else
            return -1;
        for (int i = 1; i < len; i++) {
            int c = getc(file());
            if ((c & 0xc0) != 0x80)
                return -1;
            __c <<= 6;
            __c |= (c & 0x3f);
        }
        return __c;
    }

  template<>
    stdio_sync_filebuf<char32_t>::int_type
    stdio_sync_filebuf<char32_t>::syncungetc(int_type __c)
    {
        char c8[7];
        c32toc8(__c, c8);
        for (int i = 0; c8[i]; i++)
            if ( ungetc(c8[i], file()) == EOF )
                return traits_type::eof();
        return __c;
    }

  template<>
    stdio_sync_filebuf<char32_t>::int_type
    stdio_sync_filebuf<char32_t>::syncputc(int_type __c)
    {
        char c8[7];
        c32toc8(__c, c8);
        for (int i = 0; c8[i]; i++)
            if ( putc(c8[i], file()) == EOF )
                return traits_type::eof();
        return __c;
    }
