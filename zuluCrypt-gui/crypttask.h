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
#ifndef CryptTask_H
#define CryptTask_H

#include <QObject>
#include <QThread>
#include <QString>

class CryptTask : public QThread
{
	Q_OBJECT
public:
	typedef enum{
		unset,
		quit,
		success,
		openMapperReadFail,
		openMapperWriteFail,
		openMapperFail,
		closeMapperFail,
		openKeyFileReadFail,
		QProcessFail,
		md5Fail,
		md5Pass,
		wrongKey,
		encryptSuccess,
		decryptSuccess,
		destinationFileExists,
		createFileFail,
		OpenSourceFail,
		OpenDestinationFail
	}status;
	int updateProgress( int ) ;
	CryptTask( const QString& source,const QString& dest,
			    const QString& keySource,const QString& key,const QString& task ) ;
	~CryptTask() ;
signals:
	void progressUpdate( int ) ;
	void titleUpdate( QString ) ;
	void complete( int ) ;
	void md5mismach( void ) ;
	void enableCancel( void ) ;
	void disableCancel( void ) ;
public slots:
	void terminate( void ) ;
private:
	void oldEncryptionRoutine( void ) ;
	void newEncryptionRoutine( void ) ;
	void oldDecryptionRoutine( void ) ;
	void newDecryptionRoutine( void ) ;
	void calculateMd5( const QString& path,char * result ) ;
	void run( void ) ;
	CryptTask::status encrypt( void ) ;
	CryptTask::status decrypt( void ) ;
	CryptTask::status openMapper( const QString& path ) ;
	CryptTask::status closeMapper( const QString& path ) ;
	QString m_source ;
	QString m_dest ;
	QString m_keySource ;
	QString m_key ;
	QString m_task ;
	QString m_mapperPath ;
	CryptTask::status m_status ;
};

#endif // CryptTask_H
