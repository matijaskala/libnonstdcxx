/*
 * Lexical and runtime cast
 * Copyright (C) 2014  Matija Skala <mskala@gmx.com>
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

#include "casts"

#include <locale>

template<>
std::wstring lexical_cast ( const std::string& s ) throw ( non_std::bad_lexical_cast ) {
    std::wstring wstr;
    {
        std::locale l{""};
        using cvt_t = std::codecvt< wchar_t, char, mbstate_t >;
        const cvt_t& cvt = std::use_facet< cvt_t > ( l );
        wchar_t* pwc;
        const char* pc;
        auto dest = new wchar_t[s.length() + 1];
        auto state = mbstate_t{};
        switch ( cvt.in(state, s.c_str(), s.c_str() + s.length() + 1, pc, dest, dest + s.length() + 1, pwc) )
        {
            case cvt_t::ok:
                break;
            default:
                throw non_std::bad_lexical_cast{};
        }
        wstr = dest;
        delete[] dest;
    }
    return wstr;
}
