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

class QString ;

class keystrength
{
public:
	keystrength() ;
	~keystrength() ;
	int quality( const QString& ) ;
	bool canCheckQuality( void ) ;
private:
	void * m_handle ;
};

#endif // KEYSTRENGTH_H
