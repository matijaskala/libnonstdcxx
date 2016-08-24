/*
 * Colored output
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

#include "xterm"
#include <cstdlib>

using namespace non_std;

std::string xterm::escape ( const std::string& __s ) {
    static char* TERM = getenv ( "TERM" );
    return TERM ? "[" + __s : "";
}

std::string xterm::bgcolor ( color __c ) {
    return escape ( "4" + std::to_string ( __c ) + "m" );
}

std::string xterm::fgcolor ( color __c ) {
    return escape ( "3" + std::to_string ( __c ) + "m" );
}
