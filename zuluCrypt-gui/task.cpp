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

Task::Task()
{
}

Task::Task( const QString& exe ) : m_exe( exe ),m_status( -1 ),m_partitionType( exe )
{
}

Task::Task( QTableWidget * table ) : m_table( table )
{
}

Task::Task( const QString& path,const QString& mpoint )
{
	m_path = path ;
	m_path.replace( "\"","\"\"\"" ) ;
	m_mpoint = mpoint ;
	m_mpoint.replace( "\"","\"\"\"" ) ;
	m_folderOpener = path ;
}

void Task::start( Task::action action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}

void Task::openMountPointTask()
{
	QProcess exe ;
	exe.start( QString( "%1 \"%2\"" ).arg( m_folderOpener ).arg( m_mpoint ) ) ;
	exe.waitForFinished() ;
	m_exitCode = exe.exitCode() ;
	m_exitStatus = exe.exitStatus() ;
	exe.close() ;
}

void Task::updateVolumeListTask()
{
	QProcess p ;
	p.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -L" ) ) ;
	p.waitForFinished() ;
	m_status = p.exitCode() ;

	QStringList l = QString( p.readAll() ).split( "\n" ) ;

	if( m_status ){
		return ;
	}

	p.close() ;

	int j = l.size() - 1 ;

	if( j == 0 ){
		return ;
	}
	QStringList entry ;

	for( int i = 0 ; i < j ; i++ ){
		entry = l.at( i ).split( "\t" ) ;
		if( entry.size() >= 3 ){
			emit addItemToTable( entry.at( 0 ),entry.at( 1 ),entry.at( 2 ) ) ;
		}
	}
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

void Task::runVolumeTask()
{
	QProcess p ;

	/*
	  Root user can create encrypted volumes in all partitions including system partitions.
	  Show all partitions, not only non system.
	  */
	if( m_partitionType == QString( " -N" ) && getuid() == 0 ){
		m_partitionType = QString( " -A" ) ;
	}

	p.start( QString( ZULUCRYPTzuluCrypt ) + m_partitionType + QString( " -Z" ) ) ;

	p.waitForFinished() ;

	QStringList l = QString( p.readAllStandardOutput() ).split( "\n" ) ;

	p.close() ;
	int j = l.size() - 1 ;
	if( j < 1 ){
		return ;
	}
	QStringList list ;
	QString entry ;
	for( int i = 0 ; i < j ; i++ ){
		list = l.at( i ).split( "\t" ) ;
		entry = list.at( 3 ) ;
		/*
		 * MDRAID partitions have "linux_raid_member" as their file system
		 * LVM partitions have "LVM2_member" as their file system
		 *
		 * we are not showing these partitions since we dont support them
		 */
		if( entry.contains( QString( "member" ) ) ){
			;
		}else{
			emit partitionProperties( list ) ;
		}
	}
}

void Task::run()
{
	switch( m_action ){
		case Task::exeTask              : return this->runExeTask() ;
		case Task::closeVolumeTask      : return this->runExeTask() ;
		case Task::closeAllVolumeTask   : return this->runCloseAllVolumeTask() ;
		case Task::volumePropertiesTask : return this->runVolumePropertiesTask() ;
		case Task::updateVolumeList     : return this->updateVolumeListTask() ;
		case Task::openMountPoint       : return this->openMountPointTask() ;
		case Task::volumeTask           : return this->runVolumeTask() ;
	}
}

Task::~Task()
{
	emit finished() ;
	emit finished( m_status ) ;
	emit finished( m_status,m_output ) ;
	emit finished( m_volumeProperties ) ;
	emit errorStatus( m_exitCode,m_exitStatus,m_startError ) ;
}
