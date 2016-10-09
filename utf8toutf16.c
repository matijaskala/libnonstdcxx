/*
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

#include <malloc.h>
#include <stdbool.h>
#include <unicode/utf8.h>
#include <unicode/utf16.h>
#include "char32.h"

uint8_t _c8len ( char __c );
uint_least16_t* utf8_to_utf16 ( const char* __src, uint_least16_t* __dst, size_t __len ) {
    if (!__len)
        __len = utf8_strlen(__src);
    bool alloc = !__dst;
    if (alloc)
        __dst = malloc(__len * 4 + 2);
    for (int i = 0; i < __len; i++) {
        if (U8_IS_SINGLE(*__src)) {
            __dst[i] = *(__src++);
            continue;
        }
        uint8_t len = _c8len(*__src);
        if (len == 1) {
            if (alloc)
                free(__dst);
            return NULL;
        }
        uint_least32_t __c = *__src & (0xfe >> len);
        for (int j = 1; j < len; j++) {
            int c = __src[j];
            if (!U8_IS_TRAIL(c)) {
                if (alloc)
                    free(__dst);
                return NULL;
            }
            __c <<= 6;
            __c |= (c & 0x3f);
        }
        __src += len;
        if (U16_LENGTH(__c) == 2) {
            __dst[++i] = U16_LEAD(__c);
            __c = U16_TRAIL(__c);
            __len++;
        }
        __dst[i] = __c;
    }
    __dst[__len] = 0;
    if (alloc)
        __dst = realloc(__dst, __len * 2 + 2);
    return __dst;
}
