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

#include "task.h"
#include <QDebug>

#include <QProcess>
#include <QThreadPool>
#include <QDir>
#include <QFile>

#include "../zuluCrypt-gui/lxqt_wallet/frontend/lxqt_wallet.h"
#include "../zuluCrypt-gui/utility.h"
#include "bin_path.h"

#include <unistd.h>

int Task::FileHandle::operator()( int fd )
{
	m_fd = fd ;
	return fd ;
}

Task::FileHandle::~FileHandle()
{
	if( m_fd != -1 ){
		close( m_fd ) ;
	}
}

Task::Task()
{
}

void Task::setMode( const QString& mode )
{
	m_mode = mode ;
}

void Task::setDevice( const QString& path )
{
	m_device = path ;
}

void Task::setType( const QString& type )
{
	m_type = type ;
}

void Task::setKeySource( const QString& key )
{
	m_keySource = key ;
}

void Task::setKey( const QString& key )
{
	m_key = key ;
}

void Task::setKeyPath( const QString& path )
{
	m_path = path ;
}

void Task::setMountPoint( const QString& m )
{
	m_point = m ;
	m_point.replace( "\"","\"\"\"" ) ;
}

void Task::setMakeMountPointPublic( bool opt )
{
	m_publicMount = opt ;
}

void Task::setList( const QStringList& l )
{
	m_list = l ;
}

void Task::setMountPointOpener( const QString& opener )
{
	m_folderOpener = opener ;
}

void Task::setDeviceOffSet( const QString& offset )
{
	m_deviceOffSet = offset ;
}

void Task::setDeviceType( Task::deviceType d )
{
	m_Device = d ;
}

void Task::setDeviceAction( Task::deviceAction d )
{
	m_deviceAction = d ;
}

void Task::setFunction( std::function< void() > function )
{
	m_function = function ;
}

void Task::run()
{
	switch( m_action ){
		case Task::Update              : return this->partitionList() ;
		case Task::Mount               : return this->mount() ;
		case Task::Unmount             : return this->umount() ;
		case Task::CryptoOpen          : return this->cryptoOpen() ;
		case Task::VolumeProperties    : return this->volumeProperties() ;
		case Task::VolumeMiniProperties: return this->volumeMiniProperties() ;
		case Task::CheckPermissions    : return this->checkPermissions() ;
		case Task::VolumeType          : return this->getVolumeType() ;
		case Task::checkUnMount        : return this->checkUnmount() ;
		case Task::openMountPoint      : return this->openMountPointTask() ;
		case Task::getKey              : return this->getKeyTask() ;
		case Task::sendKey             : return this->keySend() ;
		case Task::deviceProperty      : return this->deviceProperties() ;
	}
}

void Task::keySend()
{
	utility::sendKey( m_path,m_key ) ;
}

void Task::getKeyTask()
{
	m_function() ;
}

void Task::checkUnmount()
{
	QProcess p ;
	QString exe = QString( "%1 -c -d \"%2\"" ).arg( zuluMount ).arg( m_device.replace( "\"","\"\"\"" ) ) ;
	p.start( exe ) ;
	p.waitForFinished() ;
}

void Task::getVolumeType()
{
	this->getVolumeType( m_device ) ;
}

void Task::getVolumeType( const QString& device )
{
	auto _systemDevice = []( const QString& dev ){
		QProcess p ;
		QString exe = QString( "%1 -S" ).arg( zuluMount ) ;
		p.start( exe ) ;
		p.waitForFinished() ;
		QString s = QString( p.readAll() ) ;
		p.close() ;
		QStringList l = s.split( "\n" ) ;
		l.removeLast() ;

		for( const auto& it : l ){
			if( it == dev ){
				return true ;
			}
		}

		return false ;
	} ;

	QString d = device ;
	QProcess p ;
	QString exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( d.replace( "\"","\"\"\"" ) ) ;
	p.start( exe ) ;
	p.waitForFinished() ;

	QString m = p.readAll() ;
	QStringList l ;
	if( p.exitCode() == 0 ){
		l =  m.split( "\t" ) ;
		if( l.size() >= 4 ){
			if( _systemDevice( d ) ){
				emit getVolumeSystemInfo( l ) ;
			}else{
				emit getVolumeInfo( l ) ;
			}
		}
	}
}

void Task::checkPermissions()
{
	QProcess p ;
	p.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -C" ) ) ;
	p.waitForFinished() ;
	emit checkPermissions( p.exitCode() ) ;
	p.close() ;
}

void Task::partitionList()
{
	QProcess p ;
	QProcess q ;

	QStringList k ;
	QStringList j ;

	p.start( QString( "%1 -l" ).arg( zuluMount ) ) ;

	if( p.waitForFinished( 10000 ) ){
		k = QString( p.readAll() ).split( '\n' ) ;
		q.start( QString( "%1 -S" ).arg( zuluMount ) ) ;
		if( q.waitForFinished( 10000 ) ){
			j = QString( q.readAll() ).split( '\n' ) ;
		}
		q.close() ;
	}

	p.close() ;

	emit signalMountedList( k,j ) ;
}

void Task::volumeProperties()
{
	QProcess p ;
	QString exe ;

	QString device ;
	if( m_device.startsWith( "UUID" ) ){
		device = m_device ;
	}else{
		device = m_device.replace( "\"","\"\"\"" ) ;
	}
	exe = QString( "%1 -s -d \"%2\"" ).arg( zuluMount ).arg( device ) ;
	p.start( exe ) ;
	p.waitForFinished() ;

	QByteArray d = p.readAll() ;
	p.close() ;
	QStringList l = QString( d ).split( "\n" ) ;

	if( l.size() > 12 ){
		emit signalProperties( d ) ;
	}else{
		if( m_type.contains( "crypto_PLAIN\n" ) ){
			/*
			 * this could be a plain volume opened with an offset
			 */
			exe = QString( "%1 -s -o bogusNecessaryArgument -d \"%2\"" ).arg( zuluMount ).arg( device ) ;

			p.start( exe ) ;
			p.waitForFinished() ;
			d = p.readAll() ;
			QStringList l = QString( d ).split( "\n" ) ;

			if( l.size() > 12 ){
				emit signalProperties( d ) ;
			}else{
				emit signalProperties( QString( "" ) ) ;
			}
		}else{
			emit signalProperties( QString( "" ) ) ;
		}
	}
}

void Task::volumeMiniProperties()
{
	auto _loopDeviceIsGone =[]( const QString& device ){
		QDir d( "/sys/block" ) ;
		QStringList l = d.entryList() ;
		QString e ;
		QString dev = QString( "%1\n" ).arg( device ) ;
		QByteArray s ;
		QFile f ;
		for( const auto& it: l ){
			if( it.startsWith( "loop" ) ){
				e = QString( "/sys/block/%1/loop/backing_file" ).arg( it ) ;
				f.setFileName( e ) ;
				f.open( QIODevice::ReadOnly ) ;
				s = f.readAll() ;
				f.close() ;
				if( s == dev ){
					return false ;
				}
			}
		}
		return true ;
	} ;

	if( !m_device.startsWith( "UUID" ) && !m_device.startsWith( "/dev/" ) ){
		/*
		 * There is some sort of a race condition here and things do not always work as expected
		 * try to sleep for a second to see if it will help
		 */
		sleep( 1 ) ;
		if( _loopDeviceIsGone( m_device ) ){
			/*
			 * we were just asked to find properties of a loop device
			 * that no longer exists,remove it from the list in the GUI window
			 */
			emit volumeRemoved( m_device ) ;
			return ;
		}
	}

	QProcess p ;
	QString exe ;

	if( m_device.startsWith( "UUID" ) ){
		exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;
	}else{
		exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( m_device.replace( "\"","\"\"\"" ) ) ;
	}

	p.start( exe ) ;
	p.waitForFinished( -1 ) ;

	if( p.exitCode() == 0 ){
		emit signalProperties( p.readAll() ) ;
	}else{
		emit signalProperties( QString() ) ;
	}
}

void Task::cryptoOpen()
{
	QProcess p ;
	QString exe ;

	QString d = m_device.replace( "\"","\"\"\"" ) ;

	if( m_publicMount ){
		const char * arg = "%1 -M -m -d \"%2\" -z \"%3\" -e %4 %5" ;
		exe = QString( arg ).arg( zuluMount ).arg( d ).arg( m_point ).arg( m_mode ).arg( m_keySource ) ;
	}else{
		const char * arg = "%1 -m -d \"%2\" -z \"%3\" -e %4 %5" ;
		exe = QString( arg ).arg( zuluMount ).arg( d ).arg( m_point ).arg( m_mode ).arg( m_keySource ) ;
	}

	p.start( exe ) ;
	p.waitForFinished( -1 ) ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 ){
		output = output.mid( index + 1 ) ;
	}
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close() ;
}

void Task::mount()
{
	QProcess p ;
	QString exe ;

	QString d = m_device.replace( "\"","\"\"\"" ) ;

	if( m_point.isEmpty() ){
		m_point = QDir::homePath() + QString( "/" ) + d.split( "/" ).last() ;
	}

	QString additionalOptions ;
	if( !m_deviceOffSet.isEmpty() ){
		additionalOptions = QString( " -o %1 %2" ).arg( m_deviceOffSet ).arg( m_keySource ) ;
	}

	if( m_publicMount ){
		const char * arg = "%1 -M -m -d \"%2\" -e %3 -z \"%4\" %5" ;
		exe = QString( arg ).arg( zuluMount ).arg( m_device ).arg( m_mode ).arg( m_point ).arg( additionalOptions ) ;
	}else{
		const char * arg = "%1 -m -d \"%2\" -e %3 -z \"%4\" %5" ;
		exe = QString( arg ).arg( zuluMount ).arg( m_device ).arg( m_mode ).arg( m_point ).arg( additionalOptions ) ;
	}

	p.start( exe ) ;
	p.waitForFinished( -1 ) ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 ){
		output = output.mid( index + 1 ) ;
	}
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close() ;
}

void Task::umount()
{
	QProcess p ;
	QString exe ;

	QString device ;
	if( m_device.startsWith( "UUID" ) ){
		device = m_device ;
	}else{
		device = m_device.replace( "\"","\"\"\"" ) ;
	}

	exe = QString( "%1 -u -d \"%2\"" ).arg( zuluMount ).arg( device ) ;

	p.start( exe ) ;
	p.waitForFinished() ;

	int index ;

	int r = p.exitCode() ;
	QString output_1 = QString( p.readAll() ) ;
	index = output_1.indexOf( QChar( ':' ) ) ;
	output_1 = output_1.mid( index + 1 ) ;
	p.close() ;

	if( r == 0 ){
		emit signalUnmountComplete( r,output_1 ) ;
	}else{
		if( m_type.contains( "crypto_PLAIN\n" ) ){
			/*
			 * we could be trying to unmount a volume with an offset
			 */
			p.close() ;
			exe = QString( "%1 -o bogusNecessaryArgument -u -d \"%2\"" ).arg( zuluMount ).arg( device ) ;
			p.start( exe ) ;
			p.waitForFinished() ;
			r = p.exitCode() ;
			if( r == 0 ){
				output_1 = QString( p.readAll() ) ;
				index = output_1.indexOf( QChar( ':' ) ) ;
				output_1 = output_1.mid( index + 1 ) ;
				p.close() ;
				emit signalUnmountComplete( r,output_1 ) ;
			}else{
				emit signalUnmountComplete( r,output_1 ) ;
			}
		}else{
			emit signalUnmountComplete( r,output_1 ) ;
		}
	}
}

void Task::openMountPointTask()
{
	QProcess exe ;
	exe.start( QString( "%1 \"%2\"" ).arg( m_folderOpener ).arg( m_point ) ) ;
	exe.waitForFinished() ;
	m_exitCode   = exe.exitCode() ;
	m_exitStatus = exe.exitStatus() ;
}

void Task::start( Task::Action action )
{
	m_action = action ;
	QThreadPool * t = QThreadPool::globalInstance() ;
	t->setMaxThreadCount( 10 ) ;
	t->start( this ) ;
}

void Task::deviceProperties()
{
	auto _deviceAdded = [&](){
		return m_deviceAction == Task::deviceAdded ;
	} ;

	auto _mdRaidDevice = [&]( const QString& device ){

		auto _mdRaidPath = []( const QString& dev ){
			QString dev_1 ;
			QDir d( "/dev/md/" ) ;
			QDir f ;

			/*
			* wait for a while because things dont always happen as expected if we check too soon.
			*/
			sleep( 4 ) ;

			if( d.exists() ){
				QStringList l = d.entryList() ;
				QString e ;
				for( const auto& it : l ){
					dev_1 = QString( "/dev/md/" ) + it ;
					f.setPath( dev_1 ) ;
					if( f.canonicalPath() == dev ){
						return dev_1 ;
					}
				}
			}
			return dev ;
		} ;

		if( _deviceAdded() ){
			this->getVolumeType( _mdRaidPath( device ) ) ;
		}else{
			emit deviceRemoved( _mdRaidPath( device ) ) ;
		}
	} ;

	auto _dmDevice = [&]( const QString& device_1 ){

		auto _deviceMatchLVMFormat = []( const QString& device ){
			/*
			 * LVM paths have two formats,"/dev/mapper/abc-def" and "/dev/abc/def/".
			 * The pass in argument is in the form of "/dev/abc/def" and the path is assumed to be
			 * an LVM path if a corresponding "/dev/mapper/abc-def" path is found
			 *
			 */
			/*
			 * We are just being lazy here and do a simple test below and return true
			 * if the path is simply in "/dev/abc/def"
			 * format by counting the number of "/"
			 */
			QStringList l = device.split( "/" ) ;
			return l.size() == 4 ;
		} ;

		QString device = device_1 ;

		int index1 = device.lastIndexOf( "-" ) ;

		if( index1 != -1 ){

			device.replace( index1,1,QString( "/" ) ) ;

			if( _deviceMatchLVMFormat( device ) ){
				if( _deviceAdded() ) {
					this->getVolumeType( device ) ;
				}else{
					emit deviceRemoved( device ) ;
				}
			}
		}
	} ;

	auto _normalDevice = [&]( const QString& device ){

		auto _allowed_device = []( const QString& device ){

			return	device.startsWith( "/dev/sd" )  ||
				device.startsWith( "/dev/hd" )  ||
				device.startsWith( "/dev/mmc" ) ;
		} ;

		if( _allowed_device( device ) ){
			if( _deviceAdded() ) {
				this->getVolumeType( device ) ;
			}else{
				emit deviceRemoved( device ) ;
			}
		}
	} ;

	QString device = QString( "/dev/%1" ).arg( m_device ) ;

	switch( m_Device ){
		case Task::device    : _normalDevice( device ) ; break ;
		case Task::md_device : _mdRaidDevice( device ) ; break ;
		case Task::dm_device : _dmDevice( device )     ; break ;
	}
}

Task::~Task()
{
	emit done() ;
	emit errorStatus( m_exitCode,m_exitStatus,m_startError ) ;
}
