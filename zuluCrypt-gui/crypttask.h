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
#include <QRunnable>
#include <QString>

class CryptTask : public QObject,public QRunnable
{
	Q_OBJECT
public:
	explicit CryptTask( const QString& source,const QString& dest,
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
	void start( void ) ;
private:
	void calculateMd5( const QString& path,char * result ) ;
	void run( void ) ;
	int encrypt( void ) ;
	int decrypt( void ) ;
	int openMapper( const QString& path ) ;
	int closeMapper( const QString& path ) ;
	QString m_source ;
	QString m_dest ;
	QString m_keySource ;
	QString m_key ;
	QString m_task ;
	QString m_mapperPath ;
	int m_status ;
};

#endif // CryptTask_H
