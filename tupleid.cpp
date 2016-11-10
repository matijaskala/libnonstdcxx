/*
 * Information about multiple types
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

#include "tupleid"

using namespace non_std;
using namespace old_tupleid;

tuple_info::tuple_info ( _TupleIdBase* __tupleid ) : _M_tupleid{__tupleid} {}

tuple_info::tuple_info ( tuple_info&& other ) : _M_tupleid{other._M_tupleid}
{
    other._M_tupleid = nullptr;
}

tuple_info::~tuple_info()
{
    delete _M_tupleid;
}

tuple_info tuple_info::operator[] ( int i ) const
{
    return (*_M_tupleid)[i];
}

tuple_info::operator std::type_index()
{
    return static_cast<std::type_index> (*_M_tupleid);
}

std::string tuple_info::name() const
{
    return _M_tupleid->name();
}

int tuple_info::count() const
{
    return _M_tupleid->count();
}
