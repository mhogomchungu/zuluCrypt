/*
 *
 *  Copyright ( c ) 2011
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

#include "createfilethread.h"

createFileThread::createFileThread( QString file,double size )
{
	m_status = 0 ;
	m_file = file ;
	m_size = size ;
}

void createFileThread::cancelOperation()
{
	m_status = -1 ;
}

void createFileThread::start()
{
	QThreadPool::globalInstance()->start( this );
}

int createFileThread::createContainerFile( void )
{
	QFile file( m_file ) ;

	if( !file.open( QIODevice::WriteOnly ) ){
		return 0 ;
	}
	char buffer[ BLOCK_SIZE ] ;

	QFile random( QString( "/dev/urandom" ) ) ;
	random.open( QIODevice::ReadOnly ) ;

	qint64 size_written = 0 ;

	int j = 0 ;
	int k ;
	do{
		if( m_status == -1 ){
			break ;
		}
		random.read( buffer,BLOCK_SIZE ) ;
		file.write( buffer,BLOCK_SIZE ) ;
		file.flush() ;

		size_written += BLOCK_SIZE ;

		k = ( int ) ( size_written * 100 / m_size ) ;

		if( k > j ){
			emit progress( k );
		}
		j = k ;
	}while( size_written < m_size ) ;

	file.setPermissions( QFile::ReadOwner|QFile::WriteOwner ) ;
	file.close();
	random.close();

	return m_status == -1 ? -1 : 0 ;
}
#include <QDebug>
int createFileThread::createContainerFileUsinggCrypt( void )
{/*
	#define GSIZE 16

	char iv[ GSIZE ] ;
	char key[ GSIZE ] ;

	char plaintext[ GSIZE ] ;
	char ciphertext[ GSIZE ] ;

	QFile f( QString( "/dev/urandom" ) ) ;
	f.open( QIODevice::ReadOnly ) ;

	f.read( plaintext,GSIZE ) ;
	f.read( iv,GSIZE ) ;
	f.read( key,GSIZE ) ;
	f.close();

	f.setFileName( m_file ) ;
	f.open( QIODevice::WriteOnly ) ;

	gcry_cipher_hd_t hd ;

	gcry_cipher_open( &hd,GCRY_CIPHER_AES256,GCRY_CIPHER_MODE_CBC,0 ) ;

	gcry_cipher_setkey( hd,key,GSIZE ) ;

	gcry_cipher_setiv( hd,iv,GSIZE ) ;

	emit progress( 0 );

	qint64 size_written = 0 ;

	int j = 0 ;
	int k ;
	do{
		if( m_status == -1 )
			break ;

		if( gcry_cipher_encrypt( hd,ciphertext,GSIZE,plaintext,GSIZE ) != 0 ){
			qDebug() << "gcrypt_cipher_error";
		}

		f.write( ciphertext,GSIZE ) ;
		f.flush() ;

		size_written += GSIZE ;

		k = ( int ) ( size_written * 100 / m_size ) ;

		if( k > j )
			emit progress( k );
		j = k ;
	}while( size_written < m_size ) ;

	emit progress( 100 );

	f.setPermissions( QFile::ReadOwner|QFile::WriteOwner ) ;
	f.close();

	gcry_cipher_close ( hd ) ;

*/	return m_status ;

}

void createFileThread::run()
{
	/*
	 * RANDOM_SOURCE is set at createfilethread.h
	 */
	if( RANDOM_SOURCE == 0 ){

		m_status = this->createContainerFileUsinggCrypt() ;

	}else if( RANDOM_SOURCE == 1 ){
		/*
		 * write random data using data from reading "/dev/urandom", slow
		 * but dependable
		 */
		m_status = this->createContainerFile() ;
	}else if( RANDOM_SOURCE == 2 ){
		/*
		 * write raandom data using cryptsetup,much faster but
		 * hangs on some kernels when the data to be written is large enough
		 */
		this->createFile();

		if( m_status != 0 ){
			return ;
		}
		this->fillCreatedFileWithRandomData();
	}
}

void createFileThread::createFile()
{
	double i ;

	QFile file( m_file ) ;

	if( !file.open( QIODevice::WriteOnly ) ){
		return ;
	}
	emit progress( 0 );

	if( !file.resize( m_size ) ){

		char data[ BLOCK_SIZE ];

		memset( data,0,BLOCK_SIZE );

		int x = 0 ;
		int y = -1 ;

		double data_written = 0 ;

		double k = m_size / SIZE ;

		for( i = 0 ; i < k ; i++ ){
			if( m_status == -1 ){
				break ;
			}
			if( x > y ){
				emit progress( x );
				y = x ;
			}
			file.write( data,BLOCK_SIZE ) ;
			file.flush() ;
			data_written += BLOCK_SIZE ;

			x = ( int )( data_written * 100 / m_size ) ;
		}

		emit doneCreatingFile();
		emit progress( 100 );
	}

	file.setPermissions( QFile::ReadOwner|QFile::WriteOwner ) ;
	file.close();
}

void createFileThread::fillCreatedFileWithRandomData()
{
	this->openVolume()  ;
	if( m_status != 0 ){
		return ;
	}
	this->writeVolume() ;
	this->closeVolume() ;
}

void createFileThread::closeVolume()
{
	QString path = m_file ;
	path.replace( "\"","\"\"\"" ) ;
	QString exe = QString( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( path );
	QProcess p ;
	p.start( exe );
	p.waitForFinished();
	p.close();
}

void createFileThread::openVolume()
{
	QString path = m_file ;
	path.replace( "\"","\"\"\"" ) ;
	/*
	 * We do not let the cli write random data to the file by using -X( we use -J )because we want to write the random data
	 * ourselves giving us the ability to knoe exactly much data is already written
	 */
	QString exe = QString( "%1 -k -J -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( path );

	QProcess p ;
	p.start( exe );
	p.waitForFinished();
	m_status = p.exitCode() ;
	p.close();
}

void createFileThread::writeVolume()
{
	QFile path( utility::mapperPath( m_file ) ) ;

	if( !path.open( QIODevice::WriteOnly ) ){
		return ;
	}
	int j ;
	int k = -1 ;

	double data_written = 0 ;

	char data[ SIZE ];

	memset( data,0,SIZE );

	emit progress( 0 );

	while( path.write( data,SIZE ) > 0 ){

		path.flush() ;
		data_written += SIZE ;

		j = ( int )( data_written * 100 / m_size ) ;

		if( j > k ){
			emit progress( j );
			k = j ;
		}

		if( m_status == -1 ){
			break ;
		}
	}

	emit progress( 100 );
	path.close();
}

createFileThread::~createFileThread()
{
	emit exitStatus( m_status );
}
