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

improved_istream& improved_istream::get(wchar_t& __c)
{
    _M_gcount = 0;
    iostate __err = goodbit;
    sentry __cerb(*this, true);
    if (__cerb) {
        try {
            int_type __cb = rdbuf()->sbumpc();
            if (traits_type::eq_int_type(__cb, traits_type::eof()))
                __err |= eofbit;
            else {
                if (isascii(__cb)) {
                    __c = wtraits_type::to_char_type(__cb);
                    _M_gcount = 1;
                    return *this;
                }
                std::string s;
                wchar_t c;
                for(;;) {
                    s += traits_type::to_char_type(__cb);
                    int l = mbrtowc(&c, s.data(), s.size(), nullptr);
                    if (l >= 0) {
                        __c = c;
                        _M_gcount = 1;
                        return *this;
                    }
                    if (l == -1)
                        throw failure{"invalid byte sequence"};
                    __cb = traits_type::to_char_type(rdbuf()->sbumpc());
                    if (traits_type::eq_int_type(__cb, traits_type::eof()))
                        throw failure{"incomplete character"};
                }
            }
        }
        catch(...) {
            this->_M_setstate(badbit);
        }
    }
    if (!_M_gcount)
        __err |= failbit;
    if (__err)
        this->setstate(__err);
    return *this;
}

improved_istream& improved_istream::get(char16_t& __c)
{
    if (_M_trail_char16) {
        __c = _M_trail_char16;
        _M_trail_char16 = u'\0';
        _M_gcount = 1;
        return *this;
    }
    char32_t c = __c;
    get(c);
    if (!gcount())
        return *this;
    if (c > 0xffff) {
        _M_trail_char16 = U16_TRAIL(c);
        __c = U16_LEAD(c);
    }
    else
        __c = traits16_type::to_char_type(traits32_type::to_int_type(c));
    return *this;
}

improved_istream& improved_istream::get(char32_t& __c)
{
    _M_gcount = 0;
    iostate __err = goodbit;
    sentry __cerb(*this, true);
    if (__cerb) {
        try {
            int_type __cb = rdbuf()->sgetc();
            if (traits_type::eq_int_type(__cb, traits_type::eof()))
                __err |= eofbit;
            else {
                if (U8_IS_SINGLE(__cb)) {
                    __c = traits32_type::to_char_type(__cb);
                    _M_gcount = 1;
                    return *this;
                }
                uint8_t len = _c8len(__cb);
                if (len == 1)
                    throw failure{"invalid byte sequence"};
                traits32_type::int_type c = __cb & (0xfe >> len);
                for (int i = 1; i < len; i++) {
                    __cb = rdbuf()->sgetc();
                    if (traits_type::eq_int_type(__cb, traits_type::eof()))
                        throw failure{"incomplete character"};
                    if (!U8_IS_TRAIL(__cb))
                        throw failure{"invalid byte sequence"};
                    c <<= 6;
                    c |= (__cb & 0x3f);
                }
                __c = traits32_type::to_char_type(c);
                _M_gcount = 1;
            }
        }
        catch(...) {
            this->_M_setstate(badbit);
        }
    }
    if (!_M_gcount)
        __err |= failbit;
    if (__err)
        this->setstate(__err);
    return *this;
}

improved_istream& improved_istream::read(wchar_t* __s, std::streamsize __n)
{
    _M_gcount = 0;
    sentry __cerb(*this, true);
    if (__cerb)
    {
        iostate __err = goodbit;
        try {
            for (std::streamsize i = 0; i < __n; i++) {
                int_type __cb = rdbuf()->sbumpc();
                if (isascii(__cb)) {
                    __s[i] = wtraits_type::to_char_type(__cb);
                    _M_gcount++;
                    continue;
                }
                std::string s;
                wchar_t c;
                for(;;) {
                    s += traits_type::to_char_type(__cb);
                    int l = mbrtowc(&c, s.data(), s.size(), nullptr);
                    if (l >= 0) {
                        __s[i] = c;
                        _M_gcount++;
                        return *this;
                    }
                    if (l == -1)
                        throw failure{"invalid byte sequence"};
                    __cb = traits_type::to_char_type(rdbuf()->sbumpc());
                    if (traits_type::eq_int_type(__cb, traits_type::eof()))
                        throw failure{"incomplete character"};
                }
            }
            if (_M_gcount != __n)
                __err |= (eofbit | failbit);
        }
        catch(...) {
            this->_M_setstate(badbit);
        }
        if (__err)
            this->setstate(__err);
    }
    return *this;
}

improved_istream& improved_istream::read(char16_t* __s, std::streamsize __n)
{
    _M_gcount = 0;
    sentry __cerb(*this, true);
    if (__cerb)
    {
        iostate __err = goodbit;
        try {
            for (std::streamsize i = 0; i < __n; i++) {
                int_type __cb = rdbuf()->sgetc();
                if (traits_type::eq_int_type(__cb, traits_type::eof()))
                    break;
                if (U8_IS_SINGLE(__cb)) {
                    __s[i] = traits16_type::to_char_type(__cb);
                    _M_gcount++;
                    continue;
                }
                char s[7];
                int l = _c8len(__cb);
                if (l == 1)
                    throw failure{"invalid byte sequence"};
                if (rdbuf()->sgetn(s, l) != l)
                    throw failure{"incomplete character"};
                traits32_type::int_type c = s[0] & (0xfe >> l);
                for (int j = 1; j < l; j++) {
                    if (!U8_IS_TRAIL(s[j]))
                        throw failure{"invalid byte sequence"};
                    c <<= 6;
                    c |= (s[j] & 0x3f);
                }
                if (c > 0xffff) {
                    __s[i] = U16_LEAD(c);
                    _M_gcount++;
                    i++;
                    __s[i] = U16_TRAIL(c);
                }
                else
                    __s[i] = traits16_type::to_char_type(c);
                _M_gcount++;
            }
            if (_M_gcount != __n)
                __err |= (eofbit | failbit);
        }
        catch(...) {
            this->_M_setstate(badbit);
        }
        if (__err)
            this->setstate(__err);
    }
    return *this;
}

improved_istream& improved_istream::read(char32_t* __s, std::streamsize __n)
{
    _M_gcount = 0;
    sentry __cerb(*this, true);
    if (__cerb)
    {
        iostate __err = goodbit;
        try {
            for (std::streamsize i = 0; i < __n; i++) {
                int_type __cb = rdbuf()->sgetc();
                if (traits_type::eq_int_type(__cb, traits_type::eof()))
                    break;
                if (U8_IS_SINGLE(__cb)) {
                    __s[i] = traits32_type::to_char_type(__cb);
                    _M_gcount++;
                    continue;
                }
                char s[7];
                int l = _c8len(__cb);
                if (l == 1)
                    throw failure{"invalid byte sequence"};
                if (rdbuf()->sgetn(s, l) != l)
                    throw failure{"incomplete character"};
                traits32_type::int_type c = s[0] & (0xfe >> l);
                for (int j = 1; j < l; j++) {
                    if (!U8_IS_TRAIL(s[j]))
                        throw failure{"invalid byte sequence"};
                    c <<= 6;
                    c |= (s[j] & 0x3f);
                }
                __s[i] = traits32_type::to_char_type(c);
                _M_gcount++;
            }
            if (_M_gcount != __n)
                __err |= (eofbit | failbit);
        }
        catch(...) {
            this->_M_setstate(badbit);
        }
        if (__err)
            this->setstate(__err);
    }
    return *this;
}

std::streamsize improved_istream::readsome(wchar_t* __s, std::streamsize __n)
{
    _M_gcount = 0;
    sentry __cerb(*this, true);
    if (__cerb)
    {
        const std::streamsize __num = this->rdbuf()->in_avail();
        if (__num > 0) {
            if (__num < __n)
                __n = __num;
            for (int i = 0; i < __n; i++) {
                get(__s[i]);
                if (!gcount())
                    return _M_gcount = i;
            }
            return _M_gcount = __n;
        }
        else if (__num == -1)
            setstate(eofbit);
    }
    return _M_gcount;
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
//void test() { improved_istream f; char c; f.get(c); char32_t c32; char* c8p; std::move(f) >> c >> c32; std::move(f) >> &c32 >> c8p >> &c32; std::move(f) >> &c; }
