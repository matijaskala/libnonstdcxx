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

#include <unicode/utf8.h>
#include "char32.h"

uint_least32_t getc32 ( FILE* stream ) {
    int_least32_t __c = getc(stream);
    if ( __c == EOF )
        return -1;
    if (U8_IS_SINGLE(__c))
        return __c;
    if (!U8_IS_LEAD(__c))
        return -1;
    int len = 1;
    while (__c < 0xff << (6 - len))
        len++;
    __c &= 0xfe >> len;
    for (int i = 1; i < len; i++) {
        int c = getc(stream);
        if (!U8_IS_TRAIL(c))
            return -1;
        __c <<= 6;
        __c |= (c & 0x3f);
    }
    return __c;
}
