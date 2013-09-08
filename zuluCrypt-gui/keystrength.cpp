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

#include "keystrength.h"

#include <pwd.h>
#include <unistd.h>

keystrength::keystrength()
{
#if BUILD_PWQUALITY
	m_handle = pwquality_default_settings() ;
#endif
}

bool keystrength::canCheckQuality()
{
#if BUILD_PWQUALITY
	return true ;
#else
	return false ;
#endif
}

int keystrength::quality( QString key )
{
#if BUILD_PWQUALITY
	void * auxerror ;
	QByteArray keyArray = key.toAscii() ;
	int st = pwquality_check( m_handle,keyArray.constData(),NULL,NULL,&auxerror) ;
	pwquality_strerror( NULL,0,st,auxerror) ;
	return st ;
#else
	key = QString( "silence compiler warning" ) ;
	return NOT_USED ;
#endif
}

keystrength::~keystrength()
{
#if BUILD_PWQUALITY
	pwquality_free_settings( m_handle ) ;
#endif
}
