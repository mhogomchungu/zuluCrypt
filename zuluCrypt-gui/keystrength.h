/*
 *
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
 */

#ifndef KEYSTRENGTH_H
#define KEYSTRENGTH_H

#include <QString>

#include "can_build_pwquality.h"

#define NOT_USED -1234567
class keystrength
{
public:
	keystrength();
	~keystrength();
	int quality( QString ) ;
	bool canCheckQuality( void );
private:
#if BUILD_PWQUALITY
	pwquality_settings_t * m_handle ;
#endif  //BUILD_PWQUALITY
};

#endif // KEYSTRENGTH_H
