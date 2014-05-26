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
#include <QMetaObject>

#include <unistd.h>

#include "utility.h"
#include "tablewidget.h"
#include "task.h"
#include "../zuluCrypt-cli/constants.h"

Task::Task()
{
}

Task::Task( QObject * object ,const char * slotName ) : m_qObject( object ),
	m_slotName( slotName )
{

}

Task::Task( const QString& exe ) : m_exe( exe ),
	m_status( -1 ),m_path( exe ),m_partitionType( exe )
{
}

Task::Task( QTableWidget * table ) : m_table( table )
{
}

Task::Task( const QString& x,const QString& y )
{
	m_key  = y ;
	m_path = x ;
	m_folderOpener = x ;
	m_mpoint = y ;
	m_path.replace( "\"","\"\"\"" ) ;
	m_mpoint.replace( "\"","\"\"\"" ) ;
}

void Task::start( Task::action action,function_t function )
{
	m_function = function ;
	m_action   = action ;
	QThreadPool * thread = QThreadPool::globalInstance() ;
	thread->setMaxThreadCount( 10 ) ;
	thread->start( this ) ;
}

void Task::openMountPointTask()
{
	auto r       = utility::Task( QString( "%1 \"%2\"" ).arg( m_folderOpener ).arg( m_mpoint ) ) ;
	m_exitCode   = r.exitCode() ;
	m_exitStatus = r.exitStatus() ;
}

void Task::updateVolumeListTask()
{
	auto r = utility::Task( QString( ZULUCRYPTzuluCrypt ) + QString( " -L" ) ) ;
	if( r.success() ){
		QStringList l = r.splitOutput( '\n' ) ;
		QStringList entry ;
		for( const auto& it : l ){
			entry = utility::split( it,'\t' ) ;
			if( entry.size() >= 3 ){
				emit addItemToTable( entry.at( 0 ),entry.at( 1 ),entry.at( 2 ) ) ;
			}
		}
	}
}

void Task::runExeTask()
{
	auto r   = utility::Task( m_exe ) ;
	m_status = r.exitCode() ;
	m_output = r.output() ;
	if( m_action == Task::closeVolumeTask ){
		sleep( 1 ) ; // for UI effect
	}
}

void Task::runCloseAllVolumeTask()
{
	m_table->setEnabled( false ) ;
	sleep( 1 ) ; // for ui effect

	int volumeCount = m_table->rowCount() ;

	if( volumeCount > 0 ){

		QVector< QTableWidgetItem * > tableItems( volumeCount ) ;
		QTableWidgetItem ** it = tableItems.data() ;

		for( int i = 0 ; i < volumeCount ; i++ ){
			*( it + i ) = m_table->item( i,0 ) ;
		}

		for( QTableWidgetItem * it : tableItems ){
			QString device = it->text().replace( "\"","\"\"\"" ) ;
			auto r = utility::Task( QString( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( device ) ) ;
			emit taskResult( it,r.exitCode() ) ;
			sleep( 1 ) ; // for ui effect
		}
	}

	m_table->setEnabled( true ) ;
}

void Task::runVolumePropertiesTask()
{
	auto r = utility::Task( QString( "%1 -s -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( m_path ) ) ;
	QByteArray data = r.output() ;
	if( r.success() ){
		m_volumeProperties = QString( " " ) + QString( data.mid( data.indexOf( '\n' ) + 2 ) ) ;
	}else{
		m_volumeProperties = QString( "" ) ;
	}
}

void Task::runVolumeTask()
{
	/*
	  Root user can create encrypted volumes in all partitions including system partitions.
	  Show all partitions, not only non system.
	  */
	if( m_partitionType == QString( " -N" ) && getuid() == 0 ){
		m_partitionType = QString( " -A" ) ;
	}

	QString exe   = QString( ZULUCRYPTzuluCrypt ) + m_partitionType + QString( " -Z" ) ;
	QStringList l = utility::Task( exe ).splitOutput( '\n' ) ;
	QStringList list ;
	QString entry ;

	for( const auto& it : l ){
		list = utility::split( it,'\t' ) ;
		if( list.size() >= 4 ){
			entry = list.at( 3 ) ;
			/*
			 * MDRAID partitions have "linux_raid_member" as their file system
			 * LVM partitions have "LVM2_member" as their file system
			 *
			 * we are not showing these partitions since we dont support them
			 */
			if( !entry.contains( "member" ) ){
				emit partitionProperties( list ) ;
			}
		}
	}
}

void Task::LUKSSlotUsageTask()
{
	emit finished( utility::luksEmptySlots( m_path ) ) ;
}

void Task::keySend()
{
	utility::sendKey( m_path,m_key ) ;
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
		case Task::LUKSSlotUsage        : return this->LUKSSlotUsageTask() ;
		case Task::sendKey              : return this->keySend() ;
		case Task::runTask              : return this->taskRun() ;
	}
}

void Task::taskRun()
{
	m_function() ;
	QMetaObject::invokeMethod( m_qObject,m_slotName,Qt::QueuedConnection ) ;
}

Task::~Task()
{
	emit finished() ;
	emit finished( m_status ) ;
	emit finished( m_status,m_output ) ;
	emit finished( m_volumeProperties ) ;
	emit errorStatus( m_exitCode,m_exitStatus,m_startError ) ;
}

void Task::task( QObject * object,function_t f,const char * slotName )
{
	Task * t = new Task( object,slotName ) ;
	t->start( Task::runTask,f ) ;
}
