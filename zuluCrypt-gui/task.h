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

#ifndef RUNINTHREAD_H
#define RUNINTHREAD_H

#include <QRunnable>
#include <QObject>
#include <QThreadPool>

class QTableWidget ;
class QTableWidgetItem ;

class Task : public QObject,public QRunnable
{
	Q_OBJECT
public:
	typedef enum{
		exeTask,
		closeAllVolumeTask,
		closeVolumeTask,
		volumePropertiesTask
	}action ;

	explicit Task( QString exe ) ;
	Task( QString,QString ) ;
	explicit Task( QTableWidget * ) ;

	~Task();
	void start( Task::action = Task::exeTask ) ;
signals:
	void finished( QString ) ;
	void finished( int ) ;
	void finished( int,QString ) ;
	void taskResult( QTableWidgetItem *,int ) ;
private:
	void run( void ) ;
	void runExeTask( void ) ;
	void runCloseAllVolumeTask( void ) ;
	void runVolumePropertiesTask( void ) ;
	QString m_exe ;
	QString m_output ;
	int m_status ;
	QTableWidget * m_table ;
	Task::action m_action ;
	QString m_fusefs ;
	QString m_path ;
	QString m_mpoint ;
	QString m_volumeProperties ;
};

#endif // RUNINTHREAD_H

