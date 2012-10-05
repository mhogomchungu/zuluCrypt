/*
 *
 *  Copyright (c) 2012
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "getgpgkey.h"

#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <QDebug>

#define LEN 128

getgpgkey::getgpgkey( QString exe,QString key,QString keyFile )
{
	m_gpgExe = exe ;
	m_key = key.toAscii() ;
	m_keyFile = keyFile ;
	m_stop = false ;
}

void getgpgkey::start()
{
	QThreadPool::globalInstance()->start( this );
}

void getgpgkey::run()
{
	m_key.isEmpty() ? this->decryptWithOutKey() : this->decryptWithKey() ;
}

void getgpgkey::cancel()
{
	kill( m_pid,SIGTERM ) ;
	m_stop = true ;
}

void getgpgkey::decryptWithKey()
{
	this->decrypt( 0 );
}

void getgpgkey::decryptWithOutKey()
{
	this->decrypt( 1 );
}

void getgpgkey::decrypt( int type )
{
	int fd_write_to_gpg[2] ;
	int fd_read_from_gpg[2] ;

	pipe( fd_write_to_gpg );

	pipe( fd_read_from_gpg ) ;

	m_pid = fork() ;

	if( m_pid == -1 )
		return ;

	if( m_pid == 0 ){
		::close( fd_write_to_gpg[ 1 ] );
		dup2( fd_read_from_gpg[ 1 ],1 ) ;
		::close( 2 ) ;

		QByteArray fdb = QString::number( fd_write_to_gpg[ 0 ] ).toAscii() ;
		const char * fd = fdb.constData() ;

		QByteArray patharray = m_keyFile.toAscii() ;
		const char * pathChar = patharray.constData() ;

		QByteArray exe = m_gpgExe.toAscii() ;
		if( type == 0 ){
			execl( exe.constData(),"--bash","--no-tty","--yes","--no-mdc-warning",
			       "--no-verbose","--passphrase-fd",fd,"-d",pathChar,( void *)0 ) ;
		}else if( type == 1 ){
			execl( exe.constData(),"--bash","--no-tty","--yes","--no-mdc-warning",
			       "--no-verbose","-d",pathChar,( void *)0 ) ;
		}

		_Exit( 1 ); // shouldnt get here
	}else{
		::close( 2 ) ;
		::close( fd_write_to_gpg[ 0 ] );
		::close( fd_read_from_gpg[ 1 ] ) ;

		if( type == 0 ){
			write( fd_write_to_gpg[ 1 ],m_key.constData(),m_key.size() ) ;
			::close( fd_write_to_gpg[ 1 ] ) ;
		}

		char buffer[ LEN ] ;
		m_key.clear();
		int fd = fd_read_from_gpg[ 0 ] ;

		emit startingToReadData();
		ssize_t size ;
		ssize_t total = 0 ;
		int spin = 0 ;
		while( 1 ){
			if( m_stop )
				break ;
			size = read( fd,buffer,LEN ) ;
			if( size <= 0 )
				break ;
			m_key.append( buffer,size ) ;
			total = total + size  ;
			spin++ ;
			if( spin == 200 ){
				emit bytesRead( total );
				spin = 0 ;
			}
		}

		::close( fd ) ;
		waitpid( m_pid,NULL,WNOHANG ) ;
	}
}

getgpgkey::~getgpgkey()
{
	emit key( m_stop,m_key ) ;
}
