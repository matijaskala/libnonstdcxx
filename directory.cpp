/*
 * Directory contents
 * Copyright (C) 2014  Matija Skala <mskala@gmx.com>
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "directory"

#include <dirent.h>
#include <algorithm>

using namespace std;
using namespace non_std;

directory::directory ( string path )
{
    m_path = path;

    auto dir = opendir ( path.c_str() );
    if ( !dir )
        return;

    errno = 0;
    while ( auto entry = readdir ( dir ) ) {
        if ( errno )
            break;
        m_entries.push_back ( {entry->d_name,path} );
    }

    m_entries.shrink_to_fit();
    sort ( m_entries.begin(), m_entries.end(), [] (entry& a, entry&  b) { return a.name() < b.name(); } );

    closedir ( dir );
}
