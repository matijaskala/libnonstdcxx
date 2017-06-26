/*
 * Input stream that supports char16_t and char32_t
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

#include <codecvt>
#include <ostream>
#include "improved_istream"

#define U8_IS_SINGLE(c) !((c) & 0x80)
#define U8_IS_TRAIL(c) (((c) & 0xc0) == 0x80)
#define U16_LEAD(c) static_cast<char16_t>(((c) >> 10) + 0xd7c0)
#define U16_TRAIL(c) static_cast<char16_t>(((c) & 0x3ff) | 0xdc00)

using namespace non_std;

static std::uint8_t _c8len ( std::uint8_t __c ) {
    if (__c < 0xc0 || __c > 0xfd)
        return 1;
    std::uint8_t len = 1;
    while (__c >= static_cast<std::uint8_t> (0xff << (7 - len)))
        len++;
    return len;
}

improved_istream& improved_istream::operator>>(wchar_t& __c)
{
    improved_istream::sentry __cerb(*this, false);
    if (__cerb) {
        iostate __err = goodbit;
        try {
            const typename traits_type::int_type __cb = rdbuf()->sbumpc();
            if (traits_type::eq_int_type(__cb, traits_type::eof()))
                __err |= (eofbit | failbit);
            else {
                if (isascii(__cb)) {
                    __c = std::char_traits<wchar_t>::to_char_type(__cb);
                    return *this;
                }
                std::string s;
                s += traits_type::to_char_type(__cb);
                wchar_t c;
                do {
                    switch (mbrtowc(&c, s.data(), s.size(), nullptr)) {
                        case static_cast<size_t>(-1):
                            throw failure{"invalid byte sequence"};
                        case static_cast<size_t>(-2):
                            s += traits_type::to_char_type(rdbuf()->sbumpc());
                            if (traits_type::eq_int_type(s.back(), traits_type::eof()))
                                throw failure{"incomplete character"};
                            continue;
                        default:
                            break;
                    }
                } while(false);
                __c = c;
            }
        }
        catch(...) {
            _M_setstate(badbit);
        }
        if (__err)
            setstate(__err);
    }
    return *this;
}

improved_istream& improved_istream::operator>>(char16_t& __c)
{
    improved_istream::sentry __cerb(*this, false);
    if (__cerb) {
        try {
            if (_M_trail_char16) {
                __c = _M_trail_char16;
                _M_trail_char16 = u'\0';
                return *this;
            }
            char32_t c = std::char_traits<char32_t>::to_char_type(std::char_traits<char16_t>::to_int_type(__c));
            *this >> c;
            if (c > 0xffff) {
                _M_trail_char16 = U16_TRAIL(c);
                __c = U16_LEAD(c);
            }
            else
                __c = std::char_traits<char16_t>::to_char_type(std::char_traits<char32_t>::to_int_type(c));
        }
        catch(...) {
            _M_setstate(badbit);
        }
    }
    return *this;
}

improved_istream& improved_istream::operator>>(char32_t& __c)
{
    improved_istream::sentry __cerb(*this, false);
    if (__cerb) {
        iostate __err = goodbit;
        try {
            typename traits_type::int_type __cb = rdbuf()->sbumpc();
            if (traits_type::eq_int_type(__cb, traits_type::eof()))
                __err |= (eofbit | failbit);
            else {
                if (U8_IS_SINGLE(__cb)) {
                    __c = traits32_type::to_char_type(__cb);
                    return *this;
                }
                uint8_t len = _c8len(__cb);
                if (len == 1)
                    throw failure{"invalid byte sequence"};
                traits32_type::int_type c = __cb & (0xfe >> len);
                for (int i = 1; i < len; i++) {
                    __cb = rdbuf()->sbumpc();
                    if (traits_type::eq_int_type(__cb, traits_type::eof()))
                        throw failure{"incomplete character"};
                    if (!U8_IS_TRAIL(__cb))
                        throw failure{"invalid byte sequence"};
                    c <<= 6;
                    c |= (__cb & 0x3f);
                }
                __c = traits32_type::to_char_type(c);
            }
        }
        catch(...) {
            _M_setstate(badbit);
        }
        if (__err)
            setstate(__err);
    }
    return *this;
}

improved_istream& improved_istream::operator>>(std::wstring& __str)
{
    std::string s;
    *this >> s;
    wchar_t* pwc;
    const char* pc;
    auto dest = new wchar_t[s.length() + 1];
    std::mbstate_t state;
    auto& cvt = std::use_facet<std::codecvt<wchar_t,char,std::mbstate_t>>(getloc());
    if (cvt.in(state, s.data(), s.data() + s.size() + 1, pc, dest, dest + s.size() + 1, pwc) == std::codecvt_base::ok)
        __str = dest;
    delete[] dest;
    return *this;
}

improved_istream& improved_istream::operator>>(std::u16string& __str)
{
    std::string s;
    *this >> s;
    char16_t* pwc;
    const char* pc;
    auto dest = new char16_t[s.length() + 1];
    std::mbstate_t state;
    auto& cvt = std::use_facet<std::codecvt<char16_t,char,std::mbstate_t>>(getloc());
    if (cvt.in(state, s.data(), s.data() + s.size() + 1, pc, dest, dest + s.size() + 1, pwc) == std::codecvt_base::ok)
        __str = dest;
    delete[] dest;
    return *this;
}

improved_istream& improved_istream::operator>>(std::u32string& __str)
{
    std::string s;
    *this >> s;
    char32_t* pwc;
    const char* pc;
    auto dest = new char32_t[s.length() + 1];
    std::mbstate_t state;
    auto& cvt = std::use_facet<std::codecvt<char32_t,char,std::mbstate_t>>(getloc());
    if (cvt.in(state, s.data(), s.data() + s.size() + 1, pc, dest, dest + s.size() + 1, pwc) == std::codecvt_base::ok)
        __str = dest;
    delete[] dest;
    return *this;
}
