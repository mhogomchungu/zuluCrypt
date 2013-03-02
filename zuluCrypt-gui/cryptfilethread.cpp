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
#include <QDebug>
#include <fcntl.h>
#include <sys/stat.h>

cryptfilethread::cryptfilethread( QString source,QString dest,QString keySource,QString key,QString task )
{
	m_source = source ;
	m_dest = dest ;
	m_keySource = keySource ;
	m_key = key ;
	m_task = task ;
	m_status = -1 ;
}

void cryptfilethread::start()
{
	QThreadPool::globalInstance()->start( this );
}

void cryptfilethread::terminate()
{
	m_status = TERM_ST ;
}

void cryptfilethread::calculateMd5( QString path,char * result )
{
	emit titleUpdate( tr( "calculating md5sum" ) );
	emit disableCancel();
	MD5_CTX ctx ;
	MD5_Init( &ctx ) ;

	QByteArray p = path.toAscii() ;

	int fd = open( p.constData(),O_RDONLY ) ;

	if( fd != -1 ){
		struct stat st ;
		fstat( fd,&st ) ;
		void * map = mmap( 0,st.st_size,PROT_READ,MAP_PRIVATE,fd,0 ) ;
		if( map ){
			MD5_Update( &ctx,map,st.st_size ) ;
			munmap( map,st.st_size ) ;
			char digest[ 32 ] ;
			MD5_Final( ( unsigned char * )digest,&ctx ) ;
			for( int i = 0 ; i < 16 ; i++ ) {
				snprintf( &(result[i*2] ),32,"%02x",( unsigned int )digest[i] );
			}
		}
		close( fd ) ;
	}
	emit enableCancel();
}

int cryptfilethread::encrypt()
{
	QFile fd_4( m_source ) ;

	if( !fd_4.open( QIODevice::ReadOnly ) ){
		return 13 ;
	}

	QFile fd_1( m_dest ) ;

	if( !fd_1.open( QIODevice::WriteOnly ) ){
		return 10 ;
	}

	const int SIZE = 512 ;
	char buffer[ SIZE ];

	qint64 size = fd_4.size() ;
	qint64 source_size = size ;
	int i = 0 ;
	int j = -1 ;

	emit titleUpdate( tr( "creating encrypted container file" ) );

	while( size % SIZE != 0 ){
		size++ ;
	}
	size += SIZE ;

	memset( buffer,0,SIZE ) ;

	emit progressUpdate( 0 );

	if( !fd_1.resize( size ) ){
		for( qint64 size_1 = 0 ; size_1 < size ; size_1 += SIZE ){

			if( m_status == TERM_ST ){
				return TERM_ST ;
			}
			i = ( int )( size_1 * 100 / size ) ;
			if( i > j ){
				emit progressUpdate( i );
			}
			j = i ;

			fd_1.write( buffer,SIZE );
			fd_1.flush() ;
		}

		emit progressUpdate( 100 );
	}

	fd_1.close();

	int x = this->openMapper( m_dest ) ;

	if( x != 0 ){
		return x ;
	}
	QFile fd_2( m_mapperPath ) ;
	if( !fd_2.open(QIODevice::WriteOnly ) ){
		return 4 ;
	}
	QString s = QString::number( source_size ) ;

	fd_2.write( s.toAscii(),s.size() );

	fd_2.putChar( '\0' ) ;

	char bff[ 100 ] ;

	QFile fd_3( QString( "/dev/urandom" ) ) ;
	fd_3.open( QIODevice::ReadOnly ) ;
	fd_3.read( bff,100 ) ;
	fd_3.close();

	fd_2.seek( 100 ) ;

	fd_2.write( bff,100 ) ;
	fd_2.write( bff,100 ) ;

	char version[ 32 ] = { 0 };
	strcpy( version,"1.0" ) ;

	fd_2.write( version,32 ) ;

	char md5Data[ 32 ] ;

	this->calculateMd5( m_source,md5Data );

	fd_2.write( md5Data,32 ) ;

	fd_2.flush() ;

	fd_2.seek( SIZE ) ;

	j = -1 ;

	emit titleUpdate( tr( "copying data to the container file" ) );

	emit progressUpdate( 0 );

	for( qint64 size_1 = 0 ; size_1 < size ; size_1 += SIZE ){

		if( m_status == TERM_ST ){
			return TERM_ST;
		}
		i = ( int )( size_1 * 100 / size ) ;
		if( i > j ){
			emit progressUpdate( i );
		}
		j = i ;
		fd_4.read( buffer,SIZE ) ;
		fd_2.write( buffer,SIZE );
		fd_2.flush() ;
	}

	fd_2.close();

	emit progressUpdate( 100 );

	return 0 ;
}

int cryptfilethread::decrypt()
{
	if( utility::exists( m_dest ) ){
		return 5 ;
	}
	QFile fd_1( m_mapperPath ) ;
	if( !fd_1.open( QIODevice::ReadOnly ) ){
		return 4 ;
	}
	const int SIZE = 512 ;
	char buffer[SIZE] ;

	fd_1.read( buffer,SIZE ) ;

	if( memcmp( buffer + 100,buffer + 200,100 ) != 0 ){
		return 11 ;
	}

	char version[ 32 ] ;
	memcpy( version,buffer + 300,32 ) ;
	char md5sum[ 32 ] ;
	memcpy( md5sum,buffer + 332,32 ) ;

	qint64 size = atoll( buffer );
	qint64 len ;
	qint64 i = 0;
	int j = 0;
	int k = -1 ;

	QFile fd_2( m_dest ) ;
	if( !fd_2.open( QIODevice::WriteOnly) ){
		return 10 ;
	}

	emit titleUpdate( tr( "copying data from the container file" ) );

	if( size <= SIZE ){
		fd_1.read( buffer,size ) ;
		fd_2.write( buffer,size ) ;
		fd_2.flush() ;
	}else{
		len = size / SIZE ;

		for( i = 0 ; i < len ; i++ ){

			if( m_status == TERM_ST ){
				return 1 ;
			}
			j = ( int )( i * 100 / len ) ;

			if( j > k ){
				emit progressUpdate( j );
			}
			k = j ;

			fd_1.read( buffer,SIZE ) ;
			fd_2.write( buffer,SIZE ) ;
			fd_2.flush() ;
		}

		len = size - ( i * SIZE ) ;

		fd_1.read( buffer,len ) ;
		fd_2.write( buffer,len ) ;
		fd_2.flush() ;
	}

	emit progressUpdate( 100 );

	char md5Data[ 32 ] ;

	char version_1[ 32 ] = { '\0' };
	strcpy( version_1,"1.0" ) ;

	m_status = 1 ;

	if( memcmp( version,version_1,32 ) == 0 ){
		/*
		 * we are decrypting a volume using new format that embed md5 checksum of the data
		 */
		this->calculateMd5( m_dest,md5Data );

		if( memcmp( md5sum,md5Data,32 ) != 0 ){
			m_status = 1000 ;
		}
	}

	return 0 ;
}

int cryptfilethread::openMapper( QString path )
{
	m_mapperPath = utility::mapperPath( path ) ;

	path.replace( "\"","\"\"\"" ) ;
	m_key.replace( "\"","\"\"\"" ) ;

	QString e = QString( "%1 -J %3 \"%4\" -d \"%5\"" ).arg( ZULUCRYPTzuluCrypt ).arg( m_keySource ).arg( m_key ).arg( path );
	QProcess exe ;

	exe.start( e );
	exe.waitForFinished();

	int st = exe.exitCode() ;
	exe.close();
	return st ;
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
	if( m_task == QString( "-E" ) ){
		m_status = this->encrypt();

		if( m_status == 10 || m_status == 13 ){
			;
		}else if( m_status == 0 ){
			this->closeMapper( m_dest ) ;
			QFile::setPermissions( m_dest,QFile::ReadOwner|QFile::WriteOwner ) ;
		}else{
			this->closeMapper( m_dest ) ;
			QFile::remove( m_dest ) ;
		}
	}else{
		m_status = this->openMapper( m_source ) ;
		if( m_status == 0 ){
			this->decrypt();
			this->closeMapper( m_source ) ;
			if( m_status == TERM_ST ){
				QFile::remove( m_dest ) ;
			}else if( m_status == 1 || m_status == 1000 ){
				QFile::setPermissions( m_dest,QFile::ReadOwner|QFile::WriteOwner ) ;
			}
		}
	}
}

cryptfilethread::~cryptfilethread()
{
	emit complete( m_status ) ;
}
