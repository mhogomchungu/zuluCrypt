/*
 *
 *  Copyright ( c ) 2011
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

#include <QProcess>
#include <QDebug>
#include <QTableWidget>

#include <QTableWidgetItem>
#include <QTableWidget>
#include <QVector>

#include <unistd.h>

#include "task.h"
#include "../zuluCrypt-cli/constants.h"

Task::Task( QString exe ) : m_exe( exe ),m_status( -1 )
{
}

Task::Task( QTableWidget * table ) : m_table( table )
{
}

Task::Task( QString path,QString mpoint )
{
	m_fusefs = path ;
	m_path = path.replace( "\"","\"\"\"" ) ;
	m_mpoint = mpoint ;
}

void Task::start( Task::action action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}

void Task::runExeTask()
{
	QProcess p ;
	p.start( m_exe ) ;
	p.waitForFinished( -1 ) ;
	m_status = p.exitCode() ;
	m_output = QString( p.readAllStandardOutput() ) ;
	p.close() ;

	if( m_action == Task::closeVolumeTask ){
		sleep( 1 ) ; // for UI effect
	}
}

void Task::runCloseAllVolumeTask()
{
	m_table->setEnabled( false ) ;
	sleep( 1 ) ; // for ui effect

	int volumeCount = m_table->rowCount() ;

	if( volumeCount < 1 ){
		m_table->setEnabled( true ) ;
		return ;
	}

	QVector< QTableWidgetItem *> tableItems( volumeCount ) ;

	QTableWidgetItem ** it = tableItems.data() ;

	for( int i = 0 ; i < volumeCount ; i++ ){
		it[ i ] = m_table->item( i,0 ) ;
	}

	QProcess p ;
	QString exe ;
	QString device ;

	for( int i = 0 ; i < volumeCount ; i++ ){
		device = it[ i ]->text().replace( "\"","\"\"\"" ) ;
		exe = QString( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( device ) ;
		p.start( exe ) ;
		p.waitForFinished() ;
		emit taskResult( it[ i ],p.exitCode() ) ;
		p.close() ;
		sleep( 1 ) ; // for ui effect
	}

	m_table->setEnabled( true ) ;
}

void Task::runVolumePropertiesTask()
{
	QString z = QString( "%1 -s -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( m_path ) ;

	QProcess p ;
	p.start( z ) ;
	p.waitForFinished() ;
	int st = p.exitCode() ;
	QByteArray data = p.readAllStandardOutput() ;

	p.close() ;

	if( st != 0 ){
		m_volumeProperties = QString( "" ) ;
	}else{
		m_volumeProperties = QString( " " ) + QString( data.mid( data.indexOf( '\n' ) + 2 ) ) ;
	}
}

void Task::run()
{
	switch( m_action ){
		case Task::exeTask              : return this->runExeTask() ;
		case Task::closeVolumeTask      : return this->runExeTask() ;
		case Task::closeAllVolumeTask   : return this->runCloseAllVolumeTask() ;
		case Task::volumePropertiesTask : return this->runVolumePropertiesTask() ;
	}
}

Task::~Task()
{
	emit finished( m_status ) ;
	emit finished( m_status,m_output ) ;
	emit finished( m_volumeProperties ) ;
}
