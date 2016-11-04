/*
 * C++ symbol (de)mangler
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

#include "cxxabi"

#if defined __GNUC__ || defined __clang__

#include <cxxabi.h>
#include <vector>
#include <iostream>
#include <cstdlib>

using namespace std;
static constexpr auto npos = string::npos;

string non_std::demangle ( const char* symbol ) {
    using namespace abi;
    char* demangled_symbol = __cxa_demangle ( symbol, 0, 0, 0 );
    if (demangled_symbol) {
        string r = demangled_symbol;
        free ( demangled_symbol );
        return r;
    }
    else
        return "<no type>"s;
}

static string mangle_PK ( string pk )
{
    string ret;
    for ( auto i = pk.rbegin(); i != pk.rend(); i++ )
        switch ( *i )
        {
            case 'c':
                ret += 'K';
                break;
            case '*':
                ret += 'P';
                break;
        }
    return ret;
}

static string mangle_type_noPK ( string type )
{
    if ( type.empty() )
        return {};

    if ( type == "void"s )
        return "v"s;
    if ( type == "short"s )
        return "s"s;
    if ( type == "int"s )
        return "i"s;
    if ( type == "long"s )
        return "l"s;
    if ( type == "long long"s )
        return "x"s;
    if ( type == "unsigned short"s )
        return "t"s;
    if ( type == "unsigned int"s )
        return "j"s;
    if ( type == "unsigned long"s )
        return "m"s;
    if ( type == "unsigned long long"s )
        return "y"s;
    if ( type == "float"s )
        return "f"s;
    if ( type == "double"s )
        return "d"s;
    if ( type == "long double"s )
        return "e"s;
    if ( type == "bool"s )
        return "b"s;
    if ( type == "char"s )
        return "c"s;
    if ( type == "signed char"s )
        return "a"s;
    if ( type == "unsigned char"s )
        return "h"s;
    if ( type == "wchar_t"s )
        return "w"s;
    if ( type == "char16_t"s )
        return "Ds"s;
    if ( type == "char32_t"s )
        return "Di"s;
    if ( type == "std::nullptr_t"s || type == "decltype(nullptr)"s )
        return "Dn"s;
    if ( type == "__int128"s )
        return "n"s;
    if ( type == "unsigned __int128"s )
        return "o"s;
    if ( type == "__float128"s )
        return "g"s;
    if ( type == "..."s )
        return "z"s;

    return to_string(type.length()) + type;
}

string non_std::mangle_symbol (string symbol)
{
    bool is_member = false;
    string ret = "_Z";
    string n;
    vector<string> types;
    for (size_t i = 0; ; i++)
    {
        if ( symbol[i] == ' ' )
        {
            cerr << "unexpected whitespace in symbol '" << symbol << "'" << endl;
            if ( !n.empty() && ( symbol[i+1] == '_' || isalnum(symbol[i+1]) ) )
                n += ' ';
        }
        else if ( symbol[i] == '\0' )
        {
            if ( !n.empty() )
                ret += to_string(n.length());
            ret += n;
            if ( is_member )
                return ret + 'E';
            else
                return symbol;
        }
        else if ( symbol[i] == ':' )
        {
            is_member = true;
            i++;
            if ( symbol[i] != ':' )
                throw runtime_error ( "unexpected character '"s + symbol[i] + "' in symbol '"s + symbol + "'"s );
            if ( ret.length() == 2 )
            {
                ret += 'N';
                size_t p = symbol.rfind ( ')' );
                if ( i < p && p != npos )
                {
                    do {
                        p++;
                    } while ( symbol[p] == ' ' );
                    if ( symbol.substr ( p, 5 ) == "const"s )
                    {
                        ret += 'K';
                        p += 5;
                        do {
                            p++;
                        } while ( symbol[p] == ' ' );
                        if ( symbol[p] )
                            throw runtime_error ( "unexpected character '"s + symbol[p] + "' in symbol '"s + symbol + "'"s );
                    }
                    else if ( symbol[p] )
                        throw runtime_error ( "unexpected character '"s + symbol[p] + "' in symbol '"s + symbol + "'"s );
                }
            }
            if ( !n.empty() ) {
                auto tsym = to_string(n.length()) + n;
                if ( types.empty() )
                    types.push_back(tsym);
                else {
                    auto n = types.back();
                    if ( types.size() == 1 )
                        n.pop_back();
                    else
                        n = 'N' + n;
                    types.push_back(n + tsym + 'E');
                }
                ret += tsym;
                n.clear();
            }
        }
        else if ( symbol[i] == '(' )
        {
            if ( !n.empty() )
                ret += to_string(n.length());
            ret += n;
            auto params_str = symbol.substr(i);
            auto end = params_str.find_last_of(')');
            if ( end == npos )
                throw runtime_error ( "unexpected character '(' in symbol '"s + symbol + "'"s );
            if ( is_member )
                ret += 'E';
            else
            {
                auto p = params_str.c_str() + end;
                do {
                    p++;
                } while ( p[0] == ' ' );
                if ( p[0] )
                    throw runtime_error ( "unexpected character '"s + p[0] + "' in symbol '"s + symbol + "'"s );
            }
            params_str = params_str.substr ( 1, end - 1 );
            if ( params_str.empty() )
                return ret + 'v';
            vector<string> params;
            params.reserve ( params_str.length() / 2 );
            size_t c = 0;
            for ( auto t = params_str.find_first_of(','); t != npos; c = t + 1, t = params_str.find_first_of(',', c) )
                params.push_back ( params_str.substr ( c, t - c ) );
            params.push_back ( params_str.substr(c) );

            for ( auto param: params )
            {
                size_t pos;
                string type = param;
                while ( (pos = type.find ( "const" )) != npos )
                    type.erase ( pos, 5 );
                while ( (pos = type.find ( "*" )) != npos )
                    type.erase ( pos, 1 );
                while ( (pos = type.find ( "  " )) != npos )
                    type.replace ( pos, 2, " " );
                if ( type.front() == ' ' )
                    type.erase ( 0, 1 );
                if ( type.back() == ' ' )
                    type.pop_back();

                auto mangled = mangle_type_noPK(type);
                auto pk = param;
                while ( (pos = pk.find ( type )) != npos )
                    pk.erase ( pos, type.length() );

                auto mangled_PVKR = mangle_PK(pk);
                while ( mangled_PVKR.back() == 'K' || mangled_PVKR.back() == 'V' )
                    mangled_PVKR.pop_back();

                if ( mangled.length() == 1 )
                    ret += mangled_PVKR + mangled;
                else
                {
                    bool found = false;
                    for ( size_t t = 0; t < types.size(); t++ )
                        for ( size_t j = 0; j <= mangled_PVKR.length() && !found; j++ )
                            if ( types[t] == mangled_PVKR.substr(j) + mangled )
                            {
                                ret += mangled_PVKR.substr(0, j);
                                ret += 'S';
                                if ( t )
                                    ret += to_string ( t - 1 );
                                ret += '_';
                                found = true;
                            }
                    if ( !found )
                    {
                        ret += mangled_PVKR + mangled;
                        types.push_back ( mangled );
                        for ( auto i = mangled_PVKR.rbegin(); i != mangled_PVKR.rend(); i++ )
                            types.push_back ( *i + types.back() );
                    }
                }
            }

            return ret;
        }
        else if ( symbol[i] == '<' ) {
            ret += to_string(n.length());
            ret += n;
            ret += 'I';
            do {
                size_t t = i;
                i = symbol.find_first_of(",>", i);
                ret += mangle_type_noPK(symbol.substr(t + 1, i - t - 1));
            } while ( symbol[i] != '>' );
            ret += 'E';
            i++;
            n.clear();
        }
        else if ( symbol[i] == '_' || isalnum(symbol[i]) )
            n += symbol[i];
    }
}
#else // __GNUC__
string non_std::demangle ( const char* symbol ) {
	return ""s;
}
string non_std::mangle_symbol (string symbol) {
	return ""s;
}
#endif // __GNUC__
