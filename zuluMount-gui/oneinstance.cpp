#include "oneinstance.h"
#include <QDebug>

oneinstance::oneinstance( QObject * parent,QString socketPath,const char * methodName )
{
	this->setParent( parent );
	m_instanceExist = false ;
	m_serverPath = QDir::homePath() + QString( "/.zuluCrypt-socket/" ) ;
	m_methodName = methodName ;
	QDir d ;
	d.mkdir( m_serverPath ) ;

	m_serverPath += socketPath ;

	if( QFile::exists( m_serverPath ) ){
		m_instanceExist = true ;
		m_localSocket = new QLocalSocket( this ) ;
		connect( m_localSocket,SIGNAL( connected() ),this,SLOT( connected() ) ) ;
		m_localSocket->connectToServer( m_serverPath );

		if( m_localSocket->waitForConnected( 10000 ) ){
			qDebug() << "There seem to be another instance running,exiting this one";
		}else{
			qDebug() << "Previous instance seem to have crashes,trying to clean up before starting";

			/*
			 * failed to connect in 10 seconds, assuming the socketPath has a stale file
			 * and we are deleting it and hope the user will launch us again.
			 */
			QFile::remove( m_serverPath ) ;
			this->startInstance();
		}

	}else{
		this->startInstance();
	}
}

void oneinstance::startInstance()
{
	QMetaObject::invokeMethod( this->parent(),m_methodName,Qt::QueuedConnection ) ;

	m_instanceExist = false ;
	m_localServer = new QLocalServer( this ) ;
	connect( m_localServer,SIGNAL( newConnection() ),this,SLOT( gotConnection() ) ) ;

	m_localServer->setMaxPendingConnections( 100 );
	m_localServer->listen( QString( m_serverPath ) ) ;
}

void oneinstance::Exit()
{
	QCoreApplication::exit( 200 ) ;
}

void oneinstance::killProcess()
{
	QMetaObject::invokeMethod( this,"Exit",Qt::QueuedConnection ) ;
}

void oneinstance::gotConnection()
{
	QLocalSocket * s = new QLocalSocket( this ) ;
	s = m_localServer->nextPendingConnection() ;
	s->close();
	s->deleteLater();
	emit raise();
}

void oneinstance::connected()
{
	this->killProcess();
}

bool oneinstance::instanceExist()
{
	return m_instanceExist ;
}

oneinstance::~oneinstance()
{
	if( !m_instanceExist ){
		m_localServer->close();
		delete m_localServer ;
		QFile::remove( m_serverPath ) ;
	}else{
		m_localSocket->close();
		delete m_localSocket ;
	}
}
