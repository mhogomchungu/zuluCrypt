#include "auto_mount.h"
#include <QDebug>

auto_mount::auto_mount( QObject * parent ) : QThread( parent ),m_buffer( 0 )
{
}

auto_mount::~auto_mount()
{
	if( m_fdDir != -1 ){
		close( m_fdDir ) ;
	}
	if( m_buffer ){
		delete[] m_buffer ;
	}
}

bool auto_mount::deviceIsSystem( QString device )
{
	QProcess p ;
	QString exe = QString( "%1 -S" ).arg( zuluMount ) ;
	p.start( exe );
	p.waitForFinished() ;
	QString s = QString( p.readAll() ) ;
	p.close();
	QStringList l = s.split( "\n" ) ;
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){
		if( l.at( i ) == device ){
			return true ;
		}
	}
	return false ;
}

void auto_mount::volumeProperties( QString device )
{
	QProcess p ;
	QString exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( device ) ;
	p.start( exe );
	p.waitForFinished() ;
	QString m = p.readAll() ;

	if( p.exitCode() == 0 ){
		QStringList l =  m.split( "\t" ) ;
		if( l.size() >= 4 ){
			if( auto_mount::deviceIsSystem( device ) ){
				emit getVolumeSystemInfo( l ) ;
			}else{
				emit getVolumeInfo( l );
			}
		}
	}
}

void auto_mount::deviceFromDev( uint32_t mask,QString device )
{
	if( device.startsWith( QString( "/dev/sd") ) || device.startsWith( QString( "/dev/hd" ) ) || device.startsWith( QString( "mmc" ) ) ){
		if( mask & IN_CREATE ) {
			this->volumeProperties( device ) ;
		}else if( mask & IN_DELETE ){
			emit deviceRemoved( device ) ;
		}else{
			;
		}
	}else{
		;
	}
}

void auto_mount::deviceFromDevMapper( uint32_t mask,QString device )
{
	int index1 = device.lastIndexOf( "-" ) ;

	if( index1 == -1 ){
		return ;
	}

	device = device.replace( index1,1,QString( "/" ) ) ;

	if( deviceMatchLVMFormat( device ) ){
		if( mask & IN_CREATE ) {
			this->volumeProperties( device ) ;
		}else if( mask & IN_DELETE ){
			emit deviceRemoved( device ) ;
		}else{
			;
		}
	}else{
		;
	}
}

void auto_mount::run()
{
	size_t offset = sizeof( struct inotify_event ) ;
	size_t BUFF_SIZE = FILENAME_MAX + offset ;

	m_buffer = new char[ BUFF_SIZE ];

	m_fdDir = inotify_init() ;
	if( m_fdDir == -1 ){
		return ;
	}

	int dev = inotify_add_watch( m_fdDir,"/dev",IN_CREATE|IN_DELETE ) ;
	int mapper = inotify_add_watch( m_fdDir,"/dev/mapper",IN_CREATE|IN_DELETE ) ;

	if( dev == -1 || mapper == - 1){
		return ;
	}

	m_device = m_buffer + offset ;
	struct inotify_event * pevent ;
	QString device ;
	while( 1 ) {

		read( m_fdDir,m_buffer,BUFF_SIZE ) ;
		pevent = ( struct inotify_event * )m_buffer;

		if( strncmp( m_device,"sg4",3 ) == 0 ) {
			continue ;
		}

		device = QString( "/dev/" ) + m_device ;

		if( pevent->wd == dev ){
			this->deviceFromDev( pevent->mask,device );
		}else{
			this->deviceFromDevMapper( pevent->mask,device );
		}
	}
}


bool auto_mount::deviceMatchLVMFormat( QString device )
{
	if( device.contains( QString( ".udev/tmp" ) ) ){
		/*
		 * ignore what appear to be a temporary intermediate path
		 */
		return false ;
	}

	QByteArray b = device.toAscii() ;
	const char * e = b.constData() ;

	int i = 0 ;
	/*
	 * one of the two formats on LVM path is /dev/abc/def,below code checks if a volume is an LVM volume
	 * by counting the numder of "/" characters and an assumption is made that the volume is LVM if
	 * the number is 3
	 */
	while( *e ){
		if( *e == '/' ){
			i++ ;
		}
		e++ ;
	}
	return i == 3 ;
}
