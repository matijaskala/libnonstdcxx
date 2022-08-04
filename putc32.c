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

void _c32toc8 ( uint_least32_t c, char* c8 );
uint_least32_t putc32 ( uint_least32_t c, FILE* stream ) {
    char c8[7];
    _c32toc8(c, c8);
    for (int i = 0; c8[i]; i++)
        if (putc(c8[i], stream) == EOF)
            return -1;
    return c;
}
