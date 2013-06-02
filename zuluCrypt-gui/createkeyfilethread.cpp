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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "createkeyfilethread.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

createkeyfilethread::createkeyfilethread( QString path,int rng )
{
	m_path = path ;
	m_rng = rng ;
	m_cancelled = 0 ;
}

void createkeyfilethread::run()
{
	char data ;

	if( m_rng == 0 ){
		m_qfread = open( "/dev/urandom",O_RDONLY ) ;
	}else{
		m_qfread = open( "/dev/random",O_RDONLY ) ;
	}
	QByteArray path = m_path.toAscii() ;

	m_qfwrite = open( path.constData(),O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;

	for( int i = 0 ; i < 64 ; i++ ){
		do{
			read( m_qfread,&data,1 ) ;
		}while( data < 32 || data > 126 ) ;
		write( m_qfwrite,&data,1 ) ;
	}
	this->deleteLater();
}

void createkeyfilethread::cancelOperation()
{
	m_cancelled = 1 ;
	this->terminate();
	this->deleteLater();
}

createkeyfilethread::~createkeyfilethread()
{
	close( m_qfread ) ;
	close( m_qfwrite ) ;
	if( m_cancelled != 1 ){
		QFile::setPermissions( m_path,QFile::ReadOwner|QFile::WriteOwner ) ;
	}
	emit exitStatus( m_cancelled );
}
