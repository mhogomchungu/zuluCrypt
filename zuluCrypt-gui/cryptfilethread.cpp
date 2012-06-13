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

#include "cryptfilethread.h"

#define TERM_ST 12
/*
 * this source file dublicated code in ../zuluCrypt-cli/bin/file_encryption.c
 * documentation for what this header file does are there.
 */

cryptfilethread::cryptfilethread( QString source,QString dest,QString keySource,QString key,QString task )
{
	m_source = source ;
	m_dest = dest ;
	m_keySource = keySource ;
	m_key = key ;
	m_task = task ;
	m_status = 0 ;
}

void cryptfilethread::start()
{
	QThreadPool::globalInstance()->start( this );
}

void cryptfilethread::terminate()
{
	m_status = TERM_ST ;
}

int cryptfilethread::encrypt()
{
	const int SIZE = 512 ;
	char buffer[SIZE];

	QFile f( m_source ) ;
	qint64 size = f.size() ;
	QString size_1 = QString::number( size ) ;

	while( size % SIZE != 0 )
		size++ ;

	size += SIZE ;

	memset( buffer,0,SIZE ) ;

	QFile fd_1( m_dest ) ;
	fd_1.open( QIODevice::WriteOnly ) ;

	int i = 0 ;
	int j = -1 ;

	emit titleUpdate( tr( "creating encrypted container file" ) );

	for( qint64 size_1 = 0 ; size_1 < size ; size_1 += SIZE ){

		if( m_status == TERM_ST ){
			fd_1.close();
			QFile::remove( m_dest ) ;
			return m_status ;
		}
		i = ( int )( size_1 * 100 / size ) ;
		if( i > j )
			emit progressUpdate( i );
		j = i ;

		fd_1.write( buffer,SIZE );
	}

	fd_1.close();
	
	emit progressUpdate( 100 );	

	int x = this->openMapper( m_dest ) ;
	
	if( x != 0 ){
		QFile::remove( m_dest ) ;
		return x ;
	}

	QFile fd_2( m_mapperPath ) ;
	fd_2.open( QIODevice::WriteOnly ) ;

	fd_2.write( size_1.toAscii(),size_1.size() );

	fd_2.putChar( '\0' ) ;

	QFile fd_3( QString( "/dev/urandom" ) ) ;
	fd_3.open( QIODevice::ReadOnly ) ;

	QTextStream st( &fd_3 ) ;

	QByteArray bff = st.read( 100 ).toAscii() ;

	fd_3.close();

	fd_2.seek( 100 ) ;

	fd_2.write( bff,100 ) ;
	fd_2.write( bff,100 ) ;

	fd_2.seek( SIZE ) ;

	QFile fd_4( m_source ) ;
	fd_4.open( QIODevice::ReadOnly ) ;

	j = -1 ;
	
	emit titleUpdate( tr( "copying data to the container file" ) );

	for( qint64 size_1 = 0 ; size_1 < size ; size_1 += SIZE ){

		if( m_status == TERM_ST ){
			fd_4.close();
			fd_2.close();
			this->closeMapper( m_dest ) ;
			QFile::remove( m_dest ) ;
			return m_status ;
		}

		i = ( int )( size_1 * 100 / size ) ;
		if( i > j )
			emit progressUpdate( i );
		j = i ;
		fd_4.read( buffer,SIZE ) ;
		fd_2.write( buffer,SIZE );
	}
	
	emit progressUpdate( 100 );

	fd_4.close();
	fd_2.close();

	this->closeMapper( m_dest ) ;

	QFile::setPermissions( m_dest,QFile::ReadOwner|QFile::WriteOwner ) ;

	m_status = 0 ;	
	return m_status ;
}

int cryptfilethread::decrypt()
{
	const int SIZE = 512 ;
	int st = this->openMapper( m_source ) ;
	if( st != 0 )
		return st ;

	char buffer[SIZE] ;

	QFile fd_1( m_mapperPath ) ;
	fd_1.open( QIODevice::ReadOnly ) ;

	fd_1.read( buffer,SIZE ) ;

	if( memcmp( buffer+100,buffer+200,100 ) != 0 ){
		fd_1.close() ;
		this->closeMapper( m_source ) ;
		m_status = 11 ;
		return 11 ;
	}

	qint64 size = atoll( buffer );
	qint64 len ;
	qint64 i = 0;
	int j = 0;
	int k = -1 ;

	QFile fd_2( m_dest ) ;
	fd_2.open( QIODevice::WriteOnly ) ;

	emit titleUpdate( tr( "copying data from the container file" ) );

	if( size <= SIZE ){
		fd_1.read( buffer,size ) ;
		fd_2.write( buffer,size ) ;
	}else{
		len = size / SIZE ;

		for( i = 0 ; i < len ; i++ ){

			if( m_status == TERM_ST ){
				fd_1.close(); ;
				fd_2.close(); ;
				this->closeMapper( m_source ) ;
				QFile::remove( m_dest ) ;
				return m_status ;
			}

			j = ( int )( i * 100 / len ) ;

			if( j > k )
				emit progressUpdate( j );
			k = j ;

			fd_1.read( buffer,SIZE ) ;
			fd_2.write( buffer,SIZE ) ;
		}

		len = size - ( i * SIZE ) ;

		fd_1.read( buffer,len ) ;
		fd_2.write( buffer,len ) ;
	}

	emit progressUpdate( 100 );

	fd_1.close();
	fd_2.close();

	this->closeMapper( m_source ) ;

	QFile::setPermissions( m_dest,QFile::ReadOwner|QFile::WriteOwner ) ;

	m_status = 1 ;
	return m_status ;
}

int cryptfilethread::openMapper( QString path )
{
	m_mapperPath = miscfunctions::mapperPath( path ) ;

	path.replace( "\"","\"\"\"" ) ;
	m_key.replace( "\"","\"\"\"" ) ;

	QString e = QString( "%1 -J %3 \"%4\" -d \"%5\"" ).arg( ZULUCRYPTzuluCrypt ).arg( m_keySource ).arg( m_key ).arg( path );
	QProcess exe ;

	exe.start( e );
	exe.waitForFinished();

	int st = exe.exitStatus() ;
	exe.close();

	if( st != 0 ){
		m_status = st ;
		return st ;
	}

	return 0 ;
}

int cryptfilethread::closeMapper( QString path )
{
	path.replace( "\"","\"\"\"" ) ;
	QString e = QString( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( path );
	QProcess exe ;
	exe.start( e );
	exe.waitForFinished();
	int st = exe.exitStatus() ;
	exe.close();
	return st ;
}

void cryptfilethread::run()
{
	if( m_task == QString( "-E" ) )
		this->encrypt();
	else
		this->decrypt();
}

cryptfilethread::~cryptfilethread()
{
	emit done( m_status ) ;
}
