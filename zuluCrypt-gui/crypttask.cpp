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

#include "crypttask.h"

/*
 * Documentation about the encrypted container file.
 *
 * The container file is made up of two parts,the header part and the load part.
 *
 * 1. The load part.
 *    The load is stored in a container that is a multiple of 512.This is because we use 512 bytes block in encryption.
 *    The load is padded to a multiple of 512 if it is not.
 *
 * 2. The header path.
 *    The header is 512 bytes.
 *
 * 2.1. The first 100 bytes are used to store the size of the load in a format atoll() can understand.
 * 2.2  The second 100 bytes and the third 100 bytes contain the same information read from "/dev/urandom".
 *      This information is used to check if the decryption key is the same as encryption key.This is not a security
 *      feature but a convenience one.It is used to inform a user if the decryption key if not the same as encryption key.
 * 2.3  Now we are byte 300th.The next 32 bytes contains "1.0x" where "x" is 29 NULL bytes to form a 32 bytes block representing
 *      the version of the header.
 * 2.4  Now we are at byte 332th.The next 32 bytes are used to store md5sum of the load.The sum is used at
 *      decrypting time to check load integrity.
 *
 * 3.0 The rest of the header is currently not in use.
 */

#include <QFile>
#include <QThreadPool>
#include <QProcess>
#include <QDebug>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "md5/md5.h"
#include <sys/mman.h>

#include "utility.h"
#include "task.h"
#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/bash_special_chars.h"
#include "lxqt_wallet/backend/lxqtwallet.h"

CryptTask::CryptTask( const QString& source,const QString& dest,
		      const QString& keySource,const QString& key,const QString& task )
{
	m_source = source ;
	m_dest = dest ;
	m_keySource = keySource ;
	m_key = key ;
	m_task = task ;
	m_status = CryptTask::unset ;
}

void CryptTask::start()
{
	if( m_keySource == QString( "-p" ) ){
		if( m_dest.endsWith( QString( ".zc" ) ) || m_source.endsWith( QString( ".zc" ) ) ){
			QString sockpath = utility::keyPath() ;

			Task * t = new Task( sockpath,m_key ) ;
			t->start( Task::sendKey ) ;

			m_keySource = QString( "-f" ) ;
			m_key = sockpath ;
		}
	}

	QThreadPool::globalInstance()->start( this ) ;
}

void CryptTask::terminate()
{
	m_status = CryptTask::quit ;
}

void CryptTask::calculateMd5( const QString& path,char * result )
{
	emit titleUpdate( tr( "calculating md5sum" ) ) ;
	emit disableCancel() ;

	zuluCryptMD5_CTX ctx ;
	zuluCryptMD5_Init( &ctx ) ;

	QByteArray p = path.toLatin1() ;

	int fd = open( p.constData(),O_RDONLY ) ;

	if( fd != -1 ){
		struct stat st ;
		fstat( fd,&st ) ;
		void * map = mmap( 0,st.st_size,PROT_READ,MAP_PRIVATE,fd,0 ) ;
		if( map != MAP_FAILED ){
			zuluCryptMD5_Update( &ctx,map,st.st_size ) ;
			munmap( map,st.st_size ) ;
			char digest[ 32 ] ;
			zuluCryptMD5_Final( ( unsigned char * )digest,&ctx ) ;
			for( int i = 0 ; i < 16 ; i++ ) {
				snprintf( &(result[i*2] ),32,"%02x",( unsigned int )digest[i] ) ;
			}
		}
		close( fd ) ;
	}
	emit enableCancel() ;
}

CryptTask::status CryptTask::encrypt()
{
	QFile fd_4( m_source ) ;

	if( !fd_4.open( QIODevice::ReadOnly ) ){
		return CryptTask::OpenSourceFail ;
	}

	QFile fd_1( m_dest ) ;

	if( !fd_1.open( QIODevice::WriteOnly ) ){
		return CryptTask::OpenDestinationFail ;
	}

	const int SIZE = 512 ;
	char buffer[ SIZE ] ;

	qint64 size = fd_4.size() ;
	qint64 source_size = size ;
	int i = 0 ;
	int j = -1 ;

	emit titleUpdate( tr( "creating encrypted container file" ) ) ;

	while( size % SIZE != 0 ){
		size++ ;
	}
	size += SIZE ;

	memset( buffer,0,SIZE ) ;

	emit progressUpdate( 0 ) ;

	if( !fd_1.resize( size ) ){
		for( qint64 size_1 = 0 ; size_1 < size ; size_1 += SIZE ){

			if( m_status == CryptTask::quit ){
				return CryptTask::quit ;
			}else{
				i = ( int )( size_1 * 100 / size ) ;
				if( i > j ){
					emit progressUpdate( i ) ;
				}
				j = i ;

				fd_1.write( buffer,SIZE ) ;
				fd_1.flush() ;
			}
		}

		emit progressUpdate( 100 ) ;
	}

	fd_1.close() ;

	if( this->openMapper( m_dest ) != CryptTask::success ){
		return CryptTask::openMapperFail ;
	}

	QFile fd_2( m_mapperPath ) ;
	if( !fd_2.open(QIODevice::WriteOnly ) ){
		return CryptTask::openMapperWriteFail ;
	}

	QString s = QString::number( source_size ) ;

	fd_2.write( s.toLatin1(),s.size() ) ;

	fd_2.putChar( '\0' ) ;

	char bff[ 100 ] ;

	QFile fd_3( QString( "/dev/urandom" ) ) ;
	fd_3.open( QIODevice::ReadOnly ) ;
	fd_3.read( bff,100 ) ;
	fd_3.close() ;

	fd_2.seek( 100 ) ;

	fd_2.write( bff,100 ) ;
	fd_2.write( bff,100 ) ;

	char version[ 32 ] = { 0 };
	strcpy( version,"1.0" ) ;

	fd_2.write( version,32 ) ;

	char md5Data[ 32 ] ;

	this->calculateMd5( m_source,md5Data ) ;

	fd_2.write( md5Data,32 ) ;

	fd_2.flush() ;

	fd_2.seek( SIZE ) ;

	j = -1 ;

	emit titleUpdate( tr( "copying data to the container file" ) ) ;

	emit progressUpdate( 0 ) ;

	for( qint64 size_1 = 0 ; size_1 < size ; size_1 += SIZE ){

		if( m_status == CryptTask::quit ){
			return CryptTask::quit ;
		}
		i = ( int )( size_1 * 100 / size ) ;
		if( i > j ){
			emit progressUpdate( i ) ;
		}
		j = i ;
		fd_4.read( buffer,SIZE ) ;
		fd_2.write( buffer,SIZE ) ;
		fd_2.flush() ;
	}

	fd_2.close() ;

	emit progressUpdate( 100 ) ;

	return CryptTask::success ;
}

CryptTask::status CryptTask::decrypt()
{
	if( utility::exists( m_dest ) ){
		return CryptTask::destinationFileExists ;
	}
	QFile fd_1( m_mapperPath ) ;
	if( !fd_1.open( QIODevice::ReadOnly ) ){
		return CryptTask::openMapperReadFail ;
	}
	const int SIZE = 512 ;
	char buffer[SIZE] ;

	fd_1.read( buffer,SIZE ) ;

	if( memcmp( buffer + 100,buffer + 200,100 ) != 0 ){
		return CryptTask::wrongKey ;
	}

	char version[ 32 ] ;
	memcpy( version,buffer + 300,32 ) ;
	char md5sum[ 32 ] ;
	memcpy( md5sum,buffer + 332,32 ) ;

	qint64 size = atoll( buffer ) ;
	qint64 len ;
	qint64 i = 0;
	int j = 0;
	int k = -1 ;

	QFile fd_2( m_dest ) ;
	if( !fd_2.open( QIODevice::WriteOnly ) ){
		return CryptTask::createFileFail ;
	}

	emit titleUpdate( tr( "copying data from the container file" ) ) ;

	if( size <= SIZE ){
		fd_1.read( buffer,size ) ;
		fd_2.write( buffer,size ) ;
		fd_2.flush() ;
	}else{
		len = size / SIZE ;

		for( i = 0 ; i < len ; i++ ){

			if( m_status == CryptTask::quit ){
				return CryptTask::quit ;
			}
			j = ( int )( i * 100 / len ) ;

			if( j > k ){
				emit progressUpdate( j ) ;
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

	emit progressUpdate( 100 ) ;

	char md5Data[ 32 ] ;

	char version_1[ 32 ] = { '\0' };
	strcpy( version_1,"1.0" ) ;

	if( memcmp( version,version_1,32 ) == 0 ){
		/*
		 * we are decrypting a volume using new format that embed md5 checksum of the data
		 */
		this->calculateMd5( m_dest,md5Data ) ;

		if( memcmp( md5sum,md5Data,32 ) != 0 ){
			return CryptTask::md5Fail ;
		}else{
			return CryptTask::md5Pass ;
		}
	}else{
		return CryptTask::md5Pass ;
	}
}

CryptTask::status CryptTask::openMapper( const QString& p )
{
	m_mapperPath = utility::mapperPath( p ) ;

	QString path = p ;
	path.replace( "\"","\"\"\"" ) ;
	m_key.replace( "\"","\"\"\"" ) ;

	QString e = QString( "%1 -J %3 \"%4\" -d \"%5\"" ).arg( ZULUCRYPTzuluCrypt ).arg( m_keySource ).arg( m_key ).arg( path ) ;
	QProcess exe ;

	exe.start( e ) ;
	exe.waitForFinished() ;

	int st = exe.exitCode() ;
	exe.close() ;
	if( st == 0 ){
		return CryptTask::success ;
	}else{
		return CryptTask::openMapperFail ;
	}
}

CryptTask::status CryptTask::closeMapper( const QString& p )
{
	QString path = p ;
	path.replace( "\"","\"\"\"" ) ;
	QString e = QString( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( path ) ;
	QProcess exe ;
	exe.start( e ) ;
	exe.waitForFinished() ;
	int st = exe.exitStatus() ;
	exe.close() ;
	if( st == 0 ){
		return CryptTask::success ;
	}else{
		return CryptTask::closeMapperFail ;
	}
}

void CryptTask::run()
{
	if( m_task == QString( "-D" ) ){
		if( m_source.endsWith( ".zc" ) ){
			/*
			 * this routine is used in zuluCrypt < 4.6.9
			 */
			this->oldDecryptionRoutine(); ;
		}else{
			this->newDecryptionRoutine() ;
		}
	}else{
		/*
		 * old routine used in zuluCrypt < 4.6.9
		 * this->oldDecryptionRoutine() ;
		 */

		this->newEncryptionRoutine() ;
	}
}

void CryptTask::oldEncryptionRoutine()
{
	m_status = this->encrypt() ;
	switch( m_status ){
	case CryptTask::OpenSourceFail :
	case CryptTask::OpenDestinationFail :
		break ;
	case CryptTask::openMapperFail :
		QFile::remove( m_dest ) ;
		break ;
	case CryptTask::openMapperWriteFail :
	case CryptTask::quit :
		this->closeMapper( m_dest ) ;
		QFile::remove( m_dest ) ;
		break ;
	case CryptTask::success :
		m_status = this->closeMapper( m_dest ) ;
		if( m_status == CryptTask::success ){
			m_status = CryptTask::encryptSuccess ;
		}
		QFile::setPermissions( m_dest,QFile::ReadOwner|QFile::WriteOwner ) ;
		break ;
	default:
		this->closeMapper( m_dest ) ;
		QFile::remove( m_dest ) ;
	}
}

void CryptTask::oldDecryptionRoutine()
{
	m_status = this->openMapper( m_source ) ;
	if( m_status == CryptTask::success ){
		m_status = this->decrypt() ;
		this->closeMapper( m_source ) ;
		if( m_status == CryptTask::quit ){
			QFile::remove( m_dest ) ;
		}else if( m_status == CryptTask::md5Pass || m_status == CryptTask::md5Fail ){
			QFile::setPermissions( m_dest,QFile::ReadOwner|QFile::WriteOwner ) ;
		}else if( m_status == CryptTask::wrongKey ){
			;
		}else{
			m_status = CryptTask::decryptSuccess ;
		}
	}
}

int CryptTask::updateProgress( int e )
{
	emit progressUpdate( e ) ;
	return m_status == CryptTask::quit ;
}

static int progress( int e,void * f )
{
	CryptTask * t = reinterpret_cast< CryptTask * >( f ) ;
	return t->updateProgress( e ) ;
}

void CryptTask::newEncryptionRoutine()
{
	if( m_keySource == QString( "-f" ) ){
		QFile f( m_key ) ;
		f.open( QIODevice::ReadOnly ) ;
		m_key = f.readAll() ;
	}

	lxqt_wallet_error r ;
	void * f = reinterpret_cast< void * >( this ) ;
	r = lxqt_wallet_create_encrypted_file( m_key.toLatin1().constData(),m_key.size(),
					       m_source.toLatin1().constData(),
					       m_dest.toLatin1().constData(),progress,f ) ;

	if( m_status == CryptTask::quit ){
		QFile::remove( m_dest ) ;
	}else if( r == lxqt_wallet_no_error ){
		m_status = CryptTask::encryptSuccess ;
	}else{
		m_status = CryptTask::createFileFail ;
	}
}

void CryptTask::newDecryptionRoutine()
{
	if( m_keySource == QString( "-f" ) ){
		QFile f( m_key ) ;
		f.open( QIODevice::ReadOnly ) ;
		m_key = f.readAll() ;
	}

	lxqt_wallet_error r ;
	void * f = reinterpret_cast< void * >( this ) ;
	r = lxqt_wallet_create_decrypted_file( m_key.toLatin1().constData(),m_key.size(),
					       m_source.toLatin1().constData(),
					       m_dest.toLatin1().constData(),progress,f ) ;

	if( m_status == CryptTask::quit ){
		QFile::remove( m_dest ) ;
	}else if( r == lxqt_wallet_wrong_password ){
		m_status = CryptTask::wrongKey ;
	}else if( r == lxqt_wallet_no_error ){
		m_status = CryptTask::decryptSuccess ;
	}else{
		m_status = CryptTask::wrongKey ;
	}
}

CryptTask::~CryptTask()
{
	emit complete( int( m_status ) ) ;
}
