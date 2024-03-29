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

#include "char32.h"
#include "unicode/utf8.h"

uint8_t _c8len ( char c );
uint_least32_t getc32 ( FILE* stream ) {
    uint_least32_t _c;
    uint8_t len;
    {
        int c = getc(stream);
        if ( c == EOF )
            return -1;
        if (U8_IS_SINGLE(c))
            return c;
        len = _c8len(c);
        if (len == 1)
            return -1;
        _c = c & (0xfe >> len);
    }
    for (int i = 1; i < len; i++) {
        int c = getc(stream);
        if (!U8_IS_TRAIL(c))
            return -1;
        _c <<= 6;
        _c |= (c & 0x3f);
    }
    return _c;
}
