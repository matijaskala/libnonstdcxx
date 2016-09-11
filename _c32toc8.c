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

#include <unicode/utf16.h>

void _c32toc8 ( uint_least32_t __c, char* c8 ) {
    if (!c8)
        return;
    if (__c < 0x80) {
        c8[0] = __c;
        c8[1] = '\0';
        return;
    }
    static uint_least16_t lead = 0;
    if (U16_IS_LEAD(__c))
        lead = __c;
    if (U16_IS_TRAIL(__c)) {
        __c = U16_GET_SUPPLEMENTARY(lead, __c);
        lead = 0;
    }
    if (lead) {
        c8[0] = '\0';
        return;
    }
    int len;
    if (__c < 0x800)
        len = 2;
    else if (__c < 0x10000)
        len = 3;
    else if (__c < 0x200000)
        len = 4;
    else if (__c < 0x4000000)
        len = 5;
    else
        len = 6;
    c8[len] = 0;
    for (int i = len - 1; i >= 0; i--) {
        c8[i] = 0x80 | (__c & 0x3f);
        __c >>= 6;
    }
    c8[0] |= 0xff << (8 - len);
}
