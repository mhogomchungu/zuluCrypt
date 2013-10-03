
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

#include "oneinstance.h"
#include <QDebug>

oneinstance::oneinstance( QObject * parent,const QString& socketPath,const char * methodName,QString device )
{
	m_device = device ;
	this->setParent( parent ) ;
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
		m_localSocket->connectToServer( m_serverPath ) ;

		if( m_localSocket->waitForConnected( 10000 ) ){
			qDebug() << tr( "There seem to be another instance running,exiting this one" ) ;
		}else{
			qDebug() << tr( "Previous instance seem to have crashes,trying to clean up before starting" ) ;

			/*
			 * failed to connect in 10 seconds, assuming the socketPath has a stale file
			 * and we are deleting it and hope the user will launch us again.
			 */
			QFile::remove( m_serverPath ) ;
			this->startInstance() ;
		}
	}else{
		this->startInstance() ;
	}
}

void oneinstance::startInstance()
{
	QMetaObject::invokeMethod( this->parent(),m_methodName,Qt::QueuedConnection ) ;

	m_instanceExist = false ;
	m_localServer = new QLocalServer( this ) ;

	connect( m_localServer,SIGNAL( newConnection() ),this,SLOT( gotConnection() ) ) ;

	m_localServer->setMaxPendingConnections( 100 ) ;
	m_localServer->listen( QString( m_serverPath ) ) ;
}

void oneinstance::Exit()
{
	QCoreApplication::exit( 200 ) ;
}

void oneinstance::setDevice( QString device )
{
	m_device = device ;
}

void oneinstance::killProcess()
{
	QMetaObject::invokeMethod( this,"Exit",Qt::QueuedConnection ) ;
}

void oneinstance::gotConnection()
{
	QLocalSocket * s = m_localServer->nextPendingConnection() ;
	s->waitForReadyRead() ;
	QByteArray data = s->readAll() ;
	s->close() ;
	s->deleteLater() ;
	if( data.isEmpty() ){
		emit raise() ;
	}else{
		emit raiseWithDevice( data ) ;
	}
}

void oneinstance::connected()
{
	if( !m_device.isEmpty() ){
		m_localSocket->write( m_device.toAscii() ) ;
		m_localSocket->waitForBytesWritten() ;
	}

	this->killProcess() ;
}

bool oneinstance::instanceExist()
{
	return m_instanceExist ;
}

oneinstance::~oneinstance()
{
	if( !m_instanceExist ){
		m_localServer->close() ;
		delete m_localServer ;
		QFile::remove( m_serverPath ) ;
	}else{
		m_localSocket->close() ;
		delete m_localSocket ;
	}
}
