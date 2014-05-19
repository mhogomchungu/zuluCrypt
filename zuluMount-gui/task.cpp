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

QStringList Task::updateVolumeList()
{
	QProcess p ;
	QString exe = QString( "%1 -E" ).arg( zuluMount ) ;
	p.start( exe ) ;
	p.waitForFinished( -1 ) ;
	return utility::split( p.readAll() ) ;
}

void Task::run()
{
	switch( m_action ){
		case Task::Update              : return this->VolumeList() ;
		case Task::Mount               : return this->mount() ;
		case Task::Unmount             : return this->umount() ;
		case Task::CryptoOpen          : return this->cryptoOpen() ;
		case Task::VolumeProperties    : return this->volumeProperties() ;
		case Task::VolumeMiniProperties: return this->volumeMiniProperties() ;
		case Task::CheckPermissions    : return this->checkPermissions() ;
		case Task::VolumeType          : return this->getVolumeProperties() ;
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

void Task::getVolumeProperties()
{
	this->getVolumeProperties( m_device ) ;
}

bool Task::isSystemVolume( const QString& e )
{
	QProcess p ;
	QString exe = QString( "%1 -S" ).arg( zuluMount ) ;
	p.start( exe ) ;
	p.waitForFinished() ;
	QStringList l = utility::split( p.readAll() ) ;
	for( const auto& it : l ){
		if( it == e ){
			return true ;
		}
	}
	return false ;
}

void Task::getVolumeProperties( const QString& device )
{
	QString d = device ;
	QProcess p ;
	QString exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( d.replace( "\"","\"\"\"" ) ) ;
	p.start( exe ) ;
	p.waitForFinished() ;

	if( p.exitCode() == 0 ) {

		auto entry = new volumeEntryProperties( utility::split( p.readAll(),'\t' ) ) ;
		entry->setisSystem( this->isSystemVolume( device ) ) ;
		emit volumeMiniProperties( entry ) ;
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

void Task::VolumeList()
{
	typedef struct
	{
		bool finished ;
		QStringList data ;
	}result ;

	auto _run = []( const QString& exe ){
		result r ;
		QProcess p ;
		p.start( exe ) ;
		r.finished = p.waitForFinished( 10000 ) ;
		if( r.finished ){
			r.data = utility::split( p.readAll() ) ;
		}
		return r ;
	} ;

	auto _validEntry = []( const QString& e ){
		if( e.startsWith( "/dev/md/md-device-map" ) ){
			return false ;
		}
		if( e.contains( "\tswap\t") || e.contains( "member\t" ) ){
			return false ;
		}
		return true ;
	} ;

	auto entries = new QVector< volumeEntryProperties > ;

	result all = _run( QString( "%1 -l" ).arg( zuluMount ) ) ;

	if( all.finished ){

		result system = _run( QString( "%1 -S" ).arg( zuluMount ) ) ;

		if( system.finished ){

			for( const auto& it : all.data ){

				if( _validEntry( it ) ){
					volumeEntryProperties v( utility::split( it,'\t' ) ) ;
					v.setisSystem( system.data.contains( v.volumeName() ) ) ;
					entries->append( v ) ;
				}
			}
		}
	}

	emit signalMountedList( entries ) ;
}

void Task::volumeProperties()
{
	typedef struct
	{
		bool ok ;
		QString data ;
	}result ;

	QString exe ;
	QString device ;

	if( m_device.startsWith( "UUID" ) ){
		device = m_device ;
	}else{
		device = m_device.replace( "\"","\"\"\"" ) ;
	}

	exe = QString( "%1 -s -d \"%2\"" ).arg( zuluMount ).arg( device ) ;

	auto _run = []( const QString& exe ){
		result r ;
		QProcess p ;
		p.start( exe ) ;
		p.waitForFinished() ;
		r.data = p.readAll() ;
		r.ok = utility::split( r.data ).size() > 12 ;
		return r ;
	} ;

	result r = _run( exe ) ;

	if( r.ok ){
		emit signalProperties( r.data ) ;
	}else{
		if( m_type.contains( "crypto_PLAIN\n" ) ){
			/*
			 * this could be a plain volume opened with an offset
			 */
			exe = QString( "%1 -s -o bogusNecessaryArgument -d \"%2\"" ).arg( zuluMount ).arg( device ) ;

			r = _run( exe ) ;
			if( r.ok ){
				emit signalProperties( r.data ) ;
			}else{
				emit signalProperties( "" ) ;
			}
		}else{
			emit signalProperties( "" ) ;
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
		auto entry = new volumeEntryProperties( utility::split( p.readAll(),'\t' ) ) ;
		entry->setisSystem( this->isSystemVolume( m_device ) ) ;
		emit volumeMiniProperties( entry ) ;
	}else{
		emit volumeMiniProperties( nullptr ) ;
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
		m_point = QDir::homePath() + QString( "/" ) + utility::split( d,'/' ).last() ;
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
			this->getVolumeProperties( _mdRaidPath( device ) ) ;
		}else{
			emit deviceRemoved( _mdRaidPath( device ) ) ;
		}
	} ;

	auto _dmDevice = [&]( const QString& device ){

		auto _convertLVM = []( const QString& volume ){

			QByteArray e = volume.toLatin1() ;

			char * begin = e.data() ;
			char * end = begin + e.size() ;

			char * it = begin + 3 ;

			char * k ;
			char * c ;
			char * d ;

			QString z ;

			while( it < end ){

				k = it - 2 ;
				c = it - 1 ;
				d = it ;
				it++ ;

				if( *k != '-' && *c == '-' && *d != '-' ){
					*c = '/' ;
					z = e ;
					while( z.contains( "--" ) ){
						z = z.replace( "--","-" ) ;
					}
				}
			}

			return z ;
		} ;

		QString z = _convertLVM( device ) ;
		if( !z.isEmpty() ){
			if( _deviceAdded() ) {
				this->getVolumeProperties( z ) ;
			}else{
				emit deviceRemoved( z ) ;
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
				this->getVolumeProperties( device ) ;
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
