/*
 *  Copyright ( c ) 2012
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include "keystrength.h"
#include "can_build_pwquality.h"

#include <pwd.h>
#include <unistd.h>

#include <QString>

#if BUILD_PWQUALITY

#define PWQUALITY_CAST( x ) reinterpret_cast< pwquality_settings_t * >( x )

keystrength::keystrength()
{
	m_handle = PWQUALITY_CAST( pwquality_default_settings() ) ;
}

bool keystrength::canCheckQuality()
{
	return true ;
}

int keystrength::quality( const QString& key )
{
	return pwquality_check( PWQUALITY_CAST( m_handle ),key.toAscii().constData(),NULL,NULL,NULL ) ;
}

keystrength::~keystrength()
{
	pwquality_free_settings( PWQUALITY_CAST( m_handle ) ) ;
}

#else

keystrength::keystrength()
{
}

bool keystrength::canCheckQuality()
{
	return false ;
}

int keystrength::quality( const QString& key )
{
	Q_UNUSED( key ) ;
	return -1 ;
}

keystrength::~keystrength()
{
}
#endif
