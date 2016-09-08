/*
 * 32-bit character utilities
 * Copyright (C) 2014  Matija Skala <mskala@gmx.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _CHAR32_H
#define _CHAR32_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

uint_least32_t getc32 ( FILE* stream );
uint_least32_t putc32 ( uint_least32_t __c, FILE* stream );
uint_least32_t ungetc32 ( uint_least32_t __c, FILE* stream );

uint_least16_t getc16 ( FILE* stream );

#ifdef __cplusplus
}
#endif

#endif // _CHAR32_H
