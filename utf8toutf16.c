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
#include "char32.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"

uint8_t _c8len ( char _c );
uint_least16_t* utf8_to_utf16 ( const char* src, uint_least16_t* dst, size_t len ) {
    if (!len)
        len = utf8_strlen(src);
    bool alloc = !dst;
    if (alloc)
        dst = malloc(len * 4 + 2);
    for (size_t i = 0; i < len; i++) {
        if (U8_IS_SINGLE(*src)) {
            dst[i] = *(src++);
            continue;
        }
        uint8_t len = _c8len(*src);
        if (len == 1) {
            if (alloc)
                free(dst);
            return NULL;
        }
        uint_least32_t _c = *src & (0xfe >> len);
        for (int j = 1; j < len; j++) {
            int c = src[j];
            if (!U8_IS_TRAIL(c)) {
                if (alloc)
                    free(dst);
                return NULL;
            }
            _c <<= 6;
            _c |= (c & 0x3f);
        }
        src += len;
        if (U16_LENGTH(_c) == 2) {
            dst[++i] = U16_LEAD(_c);
            _c = U16_TRAIL(_c);
            len++;
        }
        dst[i] = _c;
    }
    dst[len] = 0;
    if (alloc)
        dst = realloc(dst, len * 2 + 2);
    return dst;
}
