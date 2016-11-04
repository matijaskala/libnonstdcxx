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

std::string non_std::demangle ( const char* symbol ) {
    using namespace abi;
    char* demangled_symbol = __cxa_demangle ( symbol, 0, 0, 0 );
    std::string r;
    if (demangled_symbol) {
        r = demangled_symbol;
        std::free ( demangled_symbol );
    }
    else {
        r = "<no type>";
    }
    return r;
}

static std::string mangle_PK ( std::string pk )
{
    std::string ret;
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

static std::string mangle_type_noPK ( std::string type )
{
    if ( type.empty() )
        return {};

    if ( type == "void" )
        return "v";
    if ( type == "short" )
        return "s";
    if ( type == "int" )
        return "i";
    if ( type == "long" )
        return "l";
    if ( type == "long long" )
        return "x";
    if ( type == "unsigned short" )
        return "t";
    if ( type == "unsigned int" )
        return "j";
    if ( type == "unsigned long" )
        return "m";
    if ( type == "unsigned long long" )
        return "y";
    if ( type == "float" )
        return "f";
    if ( type == "double" )
        return "d";
    if ( type == "long double" )
        return "e";
    if ( type == "bool" )
        return "b";
    if ( type == "char" )
        return "c";
    if ( type == "signed char" )
        return "a";
    if ( type == "unsigned char" )
        return "h";
    if ( type == "wchar_t" )
        return "w";
    if ( type == "char16_t" )
        return "Ds";
    if ( type == "char32_t" )
        return "Di";
    if ( type == "std::nullptr_t" || type == "decltype(nullptr)" )
        return "Dn";
    if ( type == "__int128" )
        return "n";
    if ( type == "unsigned __int128" )
        return "o";
    if ( type == "__float128" )
        return "g";
    if ( type == "..." )
        return "z";

    std::string ret;
    ret += std::to_string(type.length());
    ret += type;
    return ret;
}

std::string non_std::mangle_symbol (std::string symbol)
{
    bool is_member = false;
    std::string ret = "_Z";
    std::string n;
    std::vector<std::string> types;
    for (std::size_t i = 0; ; i++)
    {
        if ( symbol[i] == ' ' )
        {
            std::cerr << "unexpected whitespace in symbol '" << symbol << "'" << std::endl;
            if ( !n.empty() && ( symbol[i+1] == '_' || isalnum(symbol[i+1]) ) )
                n += ' ';
        }
        else if ( symbol[i] == '\0' )
        {
            if ( !n.empty() )
                ret += std::to_string(n.length());
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
                throw std::runtime_error ( std::string{} + "unexpected character '" + symbol[i] + "' in symbol '" + symbol +"'" );
            if ( ret.length() == 2 )
            {
                ret += 'N';
                std::size_t p = symbol.rfind ( ')' );
                if ( i < p && p != std::string::npos )
                {
                    do {
                        p++;
                    } while ( symbol[p] == ' ' );
                    if ( symbol.substr ( p, 5 ) == "const" )
                    {
                        ret += 'K';
                        p += 5;
                        do {
                            p++;
                        } while ( symbol[p] == ' ' );
                        if ( symbol[p] )
                            throw std::runtime_error ( std::string{} + "unexpected character '" + symbol[p] + "' in symbol '" + symbol +"'" );
                    }
                    else if ( symbol[p] )
                        throw std::runtime_error ( std::string{} + "unexpected character '" + symbol[p] + "' in symbol '" + symbol +"'" );
                }
            }
            if ( !n.empty() ) {
                auto tsym = std::to_string(n.length()) + n;
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
                ret += std::to_string(n.length());
            ret += n;
            auto params_str = symbol.substr(i);
            auto end = params_str.find_last_of(')');
            if ( end == std::string::npos )
                throw std::runtime_error ( std::string{} + "unexpected character '(' in symbol '" + symbol +"'" );
            if ( is_member )
                ret += 'E';
            else
            {
                auto p = params_str.c_str() + end;
                do {
                    p++;
                } while ( p[0] == ' ' );
                if ( p[0] )
                    throw std::runtime_error ( std::string{} + "unexpected character '" + p[0] + "' in symbol '" + symbol +"'" );
            }
            params_str = params_str.substr ( 1, end - 1 );
            if ( params_str.empty() )
                return ret + 'v';
            std::vector<std::string> params;
            params.reserve ( params_str.length() / 2 );
            std::size_t c = 0;
            for ( auto t = params_str.find_first_of(','); t != std::string::npos; c = t + 1, t = params_str.find_first_of(',', c) )
                params.push_back ( params_str.substr ( c, t - c ) );
            params.push_back ( params_str.substr(c) );

            for ( auto param: params )
            {
                std::size_t pos;
                std::string type = param;
                while ( (pos = type.find ( "const" )) != std::string::npos )
                    type.erase ( pos, 5 );
                while ( (pos = type.find ( "*" )) != std::string::npos )
                    type.erase ( pos, 1 );
                while ( (pos = type.find ( "  " )) != std::string::npos )
                    type.replace ( pos, 2, " " );
                if ( type.front() == ' ' )
                    type.erase ( 0, 1 );
                if ( type.back() == ' ' )
                    type.pop_back();

                auto mangled = mangle_type_noPK(type);
                auto pk = param;
                while ( (pos = pk.find ( type )) != std::string::npos )
                    pk.erase ( pos, type.length() );

                auto mangled_PVKR = mangle_PK(pk);
                while ( mangled_PVKR.back() == 'K' || mangled_PVKR.back() == 'V' )
                    mangled_PVKR.pop_back();

                if ( mangled.length() == 1 )
                    ret += mangled_PVKR + mangled;
                else
                {
                    bool found = false;
                    for ( std::size_t t = 0; t < types.size(); t++ )
                        for ( std::size_t j = 0; j <= mangled_PVKR.length() && !found; j++ )
                            if ( types[t] == mangled_PVKR.substr(j) + mangled )
                            {
                                ret += mangled_PVKR.substr(0, j);
                                ret += 'S';
                                if ( t )
                                    ret += std::to_string ( t - 1 );
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
            ret += std::to_string(n.length());
            ret += n;
            ret += 'I';
            do {
                std::size_t t = i;
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
std::string non_std::demangle ( const char* symbol ) {
	return "";
}
std::string non_std::mangle_symbol (std::string symbol) {
	return "";
}
#endif // __GNUC__
