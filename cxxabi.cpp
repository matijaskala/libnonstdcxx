/*
 * C++ symbol (de)mangler
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

#include "cxxabi"

#ifdef __GNUC__

#include <cxxabi.h>
#include <memory>
#include <vector>
#include <iostream>
#include <cstdlib>

std::string non_std::demangle ( const char* symbol ) {
    std::unique_ptr< char, void ( * ) ( void* ) > name ( abi::__cxa_demangle ( symbol, 0, 0, 0 ), std::free );
    return name.get();
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
    if ( type == "double" )
        return "e";
    if ( type == "char" )
        return "c";
    if ( type == "signed char" )
        return "a";
    if ( type == "unsigned char" )
        return "h";

    std::string ret;
    ret += '0' + type.length();
    ret += type;
    return ret;
}

std::string non_std::mangle_symbol (std::string symbol)
{
    bool is_member = false;
    std::string ret = "_Z";
    std::string n;
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
            {
                ret += '0' + n.length();
                ret += n;
            }
            if ( is_member )
                return ret + 'E';
            else
                return symbol;
        }
        else if ( symbol[i] == ':' )
        {
            is_member = true;
            if ( symbol[i+1] != ':' )
                throw std::runtime_error ( std::string{} + "unexpected character '" + symbol[i+1] + "' in symbol '" + symbol +"'" );
            i++;
            if ( ret.length() == 2 )
            {
                ret += 'N';
                std::size_t p;
                for ( auto t = i; t != std::string::npos; t = symbol.find ( ')', t + 1 ) )
                    p = t;
                if ( symbol[p] == ')' )
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
            ret += '0' + n.length();
            ret += n;
            n.clear();
        }
        else if ( symbol[i] == '(' )
        {
            ret += '0' + n.length();
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

            std::vector<std::string> types;
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
                ret += mangle_PK(pk);

                if ( mangled.length() == 1 )
                    ret += mangled;
                else
                {
                    bool found = false;
                    for ( std::size_t t = 0; t < types.size(); t++ )
                        if ( types[t] == mangled )
                        {
                            ret += 'S';
                            ret += '0' + t;
                            ret += '_';
                            break;
                        }
                    if ( !found )
                    {
                        types.push_back ( mangled );
                        ret += mangled;
                    }
                }
            }

            return ret;
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
