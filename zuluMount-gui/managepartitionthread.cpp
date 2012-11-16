#include "managepartitionthread.h"

managepartitionthread::managepartitionthread()
{
}

void managepartitionthread::setMode( QString mode )
{
	m_mode = mode ;
}

void managepartitionthread::setDevice( QString path )
{
	m_device = path ;
}

void managepartitionthread::setType( QString type )
{
	m_type = type ;
}

void managepartitionthread::setKeySource( QString key )
{
	m_keySource = key ;
}

void managepartitionthread::setMountPoint( QString m )
{
	m_point = m ;
}

void managepartitionthread::openPathInFileManager()
{
	QProcess p ;
	QString exe = QString( "xdg-open ") + m_point ;
	p.start( exe );
	p.waitForFinished() ;
	p.close();
}

void managepartitionthread::run()
{
	if( m_action == QString( "update") ){

		this->partitionList();

	}else if( m_action == QString( "mount" ) ){

		this->mount();

	}else if( m_action == QString( "umount" ) ){

		this->umount( m_type );

	}else if( m_action == QString( "cryptoOpen" ) ){

		this->cryptoOpen();

	}else if( m_action == QString( "volumeProperties" ) ){

		this->volumeProperties();

	}else if( m_action == QString( "volumeMiniProperties" ) ){

		this->volumeMiniProperties() ;

	}else if( m_action == QString( "openPath" ) ){

		this->openPathInFileManager();

	}else if( m_action == QString( "checkPermissions" ) ){

		this->checkPermissions();
	}
}

void managepartitionthread::checkPermissions()
{
	QProcess p ;
	p.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -C" ) );
	p.waitForFinished() ;
	emit checkPermissions( p.exitCode() );
	p.close();
}

void managepartitionthread::partitionList()
{
	QProcess p ;

	p.start( QString( "%1 -l" ).arg( zuluMount ) ) ;
	p.waitForFinished() ;

	QStringList k = QString( p.readAll() ).split( '\n' ) ;
	p.close();

	p.start( QString( "%1 -S" ).arg( zuluMount ) ) ;
	p.waitForFinished() ;

	QStringList j = QString( p.readAll() ).split( '\n' ) ;
	p.close();

	emit signalMountedList( k,j ) ;
}

void managepartitionthread::volumeProperties()
{
	QProcess p ;
	QString exe ;

	exe = QString( "%1 -s -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;

	p.start( exe );
	p.waitForFinished() ;

	QString output = QString( p.readAll() ) ;

	if( p.exitCode() ){
		QString failed ;
		emit signalProperties( failed );
	}else{
		emit signalProperties( output );
	}

	p.close();
}

void managepartitionthread::volumeMiniProperties()
{
	QProcess p ;
	QString exe ;
	sleep( 1 ) ;
	exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;

	p.start( exe );
	p.waitForFinished() ;

	QString output = QString( p.readAll() ) ;

	if( p.exitCode() ){
		QString failed ;
		emit signalProperties( failed );
	}else{
		emit signalProperties( output );
	}

	p.close();
}

void managepartitionthread::cryptoOpen()
{
	QProcess p ;
	QString exe ;

	exe = QString( "%1 -M -d \"%2\" -z \"%3\" -e %4 %5" ).arg( zuluMount ).arg( m_device ).arg( m_point ).arg( m_mode ).arg( m_keySource ) ;

	p.start( exe );
	p.waitForFinished( -1 ) ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 )
	output = output.mid( index + 1 ) ;
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close();
}

void managepartitionthread::mount()
{
	QProcess p ;
	QString exe ;

	if( m_point.isEmpty() )
		m_point = QDir::homePath() + QString( "/" ) + m_device.split( "/" ).last() ;

	exe = QString( "%1 -m -d \"%2\" -e %3 -z \"%4\"" ).arg( zuluMount ).arg( m_device ).arg( m_mode ).arg( m_point ) ;

	p.start( exe );
	p.waitForFinished() ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 )
		output = output.mid( index + 1 ) ;
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close();
}

void managepartitionthread::umount( QString type )
{
	QProcess p ;
	QString exe ;

	if( type == QString( "crypto_LUKS" ) || type == QString( "crypto_PLAIN" ) )
		exe = QString( "%1 -U -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;
	else
		exe = QString( "%1 -u -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;

	p.start( exe );
	p.waitForFinished() ;

	sleep( 1 );

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	output = output.mid( index + 1 ) ;
	emit signalUnmountComplete( p.exitCode(),output ) ;
	p.close();
}

void managepartitionthread::startAction( QString action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}
