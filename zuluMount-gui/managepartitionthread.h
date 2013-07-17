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

#ifndef MANAGEPARTITIONTHREAD_H
#define MANAGEPARTITIONTHREAD_H

#include <QProcess>
#include <QRunnable>
#include <QString>
#include <QStringList>
#include <QThreadPool>
#include <QObject>
#include <QDir>

#include "bin_path.h"
#include <unistd.h>
#include <blkid/blkid.h>

class managepartitionthread : public QObject,public QRunnable
{
	Q_OBJECT
public:
	managepartitionthread( void ) ;
	~managepartitionthread() ;
	enum Action{
		Update,
		Mount,
		Unmount,
		CryptoOpen,
		VolumeProperties,
		VolumeMiniProperties,
		OpenPath,
		CheckPermissions,
		systemdevice,
		checkUnMount,
		VolumeType
	};
	void startAction( managepartitionthread::Action ) ;
	void setMode( QString ) ;
	void setDevice( QString ) ;
	void setType( QString ) ;
	void setKeySource( QString ) ;
	void setMountPoint( QString ) ;
	void setMakeMountPointPublic( bool ) ;
	void setList( QStringList ) ;
signals:
	void signalMountComplete( int,QString ) ;
	void signalUnmountComplete( int,QString,QString ) ;
	void signalMountedList( QStringList,QStringList ) ;
	void signalProperties( QString ) ;
	void checkPermissions( int ) ;
	void getVolumeSystemInfo( QStringList ) ;
	void getVolumeInfo( QStringList ) ;
	void done( void ) ;
private:
	void checkIfSystemDevice( void );
	void checkPermissions( void );
	void openPathInFileManager( void ) ;
	void volumeMiniProperties( void ) ;
	QString volumeMiniProperties_1( void );
	void volumeProperties( void ) ;
	void cryptoMount( QString ) ;
	void cryptoOpen( void );
	void run( void ) ;
	void partitionList( void ) ;
	void mount( void ) ;
	void umount( QString ) ;
	void getVolumeType( void );
	void getVolumeSystemType( void );
	void checkUnmount( void ) ;
	managepartitionthread::Action m_action ;
	QString m_device ;
	QString m_point ;
	QString m_keySource ;
	QString m_mode ;
	QString m_type ;
	QString m_properties ;
	bool m_publicMount ;
	QStringList m_list ;
	bool m_systemDevice ;
};

#endif // MANAGEPARTITIONTHREAD_H
