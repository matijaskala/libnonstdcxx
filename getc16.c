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
#include "unicode/utf16.h"

uint_least16_t getc16 ( FILE* stream ) {
    static uint_least16_t trail = 0;
    uint_least32_t c = trail ? trail : getc32(stream);
    if (U16_LENGTH(c) == 2) {
        trail = U16_TRAIL(c);
        c = U16_LEAD(c);
    }
    return c;
}
