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

#ifndef ZULUMOUNTASK_H
#define ZULUMOUNTASK_H

#include <QObject>
#include <QRunnable>
#include <QString>
#include <QStringList>
#include <functional>
#include <QVector>

#include "volumeentryproperties.h"

namespace LxQt{
namespace Wallet {
class Wallet ;
}
}

typedef std::function< void( void ) > function_t ;

class Task : public QObject,public QRunnable
{
	Q_OBJECT
public:
	class FileHandle
	{
	public:
		int operator()( int fd ) ;
		~FileHandle() ;
	private:
		int m_fd = -1 ;
	};

	Task() ;
	~Task() ;
	enum Action{
		Update,
		Mount,
		Unmount,
		CryptoOpen,
		VolumeProperties,
		VolumeMiniProperties,
		CheckPermissions,
		checkUnMount,
		VolumeType,
		openMountPoint,
		getKey,
		sendKey,
		deviceProperty
	} ;

	enum deviceAction{
		deviceDeleted,
		deviceAdded
	} ;

	enum deviceType{
		device,
		dm_device,
		md_device,
	} ;

	static QStringList updateVolumeList( void ) ;
	void start( Task::Action ) ;
	void setMode( const QString& ) ;
	void setDevice( const QString& ) ;
	void setType( const QString& ) ;
	void setKeySource( const QString& ) ;
	void setKey( const QString& ) ;
	void setKeyPath( const QString& ) ;
	void setMountPoint( const QString& ) ;
	void setMakeMountPointPublic( bool ) ;
	void setList( const QStringList& ) ;
	void setMountPointOpener( const QString& ) ;
	void setDeviceOffSet( const QString& ) ;
	void setDeviceType( Task::deviceType ) ;
	void setDeviceAction( Task::deviceAction ) ;
	void setFunction( std::function< void( void ) > ) ;
signals:
	void errorStatus( int exitCode,int exitStatus,int startError ) ;
	void signalMountComplete( int,QString ) ;
	void signalUnmountComplete( int,QString ) ;
	void signalMountedList( QVector< volumeEntryProperties > * ) ;
	void signalProperties( QString ) ;
	void checkPermissions( int ) ;
	void getVolumeSystemInfo( QStringList ) ;
	void getVolumeInfo( QStringList ) ;
	void done( void ) ;
	void key( QString ) ;
	void volumeRemoved( QString ) ;
	void deviceRemoved( QString ) ;
private:
	void openMountPointTask( void ) ;
	void checkPermissions( void ) ;
	void volumeMiniProperties( void ) ;
	void volumeProperties( void ) ;
	void cryptoMount( QString ) ;
	void cryptoOpen( void ) ;
	void run( void ) ;
	void partitionList( void ) ;
	void mount( void ) ;
	void umount( void ) ;
	void getVolumeType( void ) ;
	void getVolumeType( const QString& ) ;
	void getVolumeSystemType( void ) ;
	void checkUnmount( void ) ;
	void getKeyTask( void ) ;
	void keySend( void ) ;
	void deviceProperties( void ) ;
	LxQt::Wallet::Wallet * m_wallet ;
	Task::Action m_action ;
	Task::deviceAction m_deviceAction ;
	Task::deviceType m_Device ;
	QString m_volumeID ;
	QString m_key ;
	QString m_device ;
	QString m_point ;
	QString m_path ;
	QString m_keySource ;
	QString m_mode ;
	QString m_type ;
	QString m_properties ;
	QString m_deviceOffSet ;
	bool m_publicMount ;
	QStringList m_list ;
	bool m_systemDevice ;
	QString m_folderOpener ;
	int m_exitCode ;
	int m_exitStatus ;
	int m_startError ;
	function_t m_function ;
};

#endif // MANAGEPARTITIONTHREAD_H
