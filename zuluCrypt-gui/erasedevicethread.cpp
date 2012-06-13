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

#include "erasedevicethread.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

erasedevicethread::erasedevicethread( QString path )
{
	m_path = path ;
	m_status = 0 ;
}

void erasedevicethread::start()
{
	QThreadPool::globalInstance()->start( this );
}

void erasedevicethread::run()
{
	m_status = this->openMapper() ;

	if( m_status != 0 )
		return ;

	this->writeJunkThroughMapper();
	this->closeMapper();
}

void erasedevicethread::writeJunkThroughMapper()
{
	QFile fd( miscfunctions::mapperPath( m_path ) ) ;
	bool b = fd.open( QIODevice::WriteOnly ) ;

	if( b == false )
		return ;

	const int SIZE = 1024 ;

	qint64 size_written = 0 ;

	char buffer[SIZE];

	memset( buffer,0,SIZE ) ;

	int i ;
	int j = -1 ;

	int f = open( m_path.toAscii().data(),O_RDONLY ) ;
	qint64 dev_size = ( qint64 ) blkid_get_dev_size( f )  ;
	close( f ) ;

	while( fd.write( buffer,SIZE ) > 0 ){

		if( m_status == 5 )
			break ;

		size_written += SIZE ;

		i = ( int ) ( size_written * 100 / dev_size ) ;

		if( i > j ){
			emit progress( i );
			j = i ;
		}
	}
	
	fd.close();
}

void erasedevicethread::closeMapper()
{
	QString exe = QString( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( m_path ) ;

	QProcess p ;
	p.start( exe );
	p.waitForFinished();
	p.close();
}

int erasedevicethread::openMapper()
{
	QString exe = QString( "%1 -J -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( m_path );

	QProcess p ;
	p.start( exe );
	p.waitForFinished();
	int st = p.exitCode() ;
	p.close();

	return st ;
}

void erasedevicethread::cancel()
{
	m_status = 5 ;
}

erasedevicethread::~erasedevicethread()
{
	emit exitStatus( m_status );
}
