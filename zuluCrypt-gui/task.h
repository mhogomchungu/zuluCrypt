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
#include <QString>
#include <QStringList>

#include "lxqt_wallet/frontend/lxqt_wallet.h"

#define COMMENT "-zuluCrypt_Comment_ID"

namespace lxqt{
namespace Wallet {
class Wallet ;
}
}

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
		volumePropertiesTask,
		updateVolumeList,
		openMountPoint,
		volumeTask,
		addKey,
		deleteKey,
		getAllKeys
	}action ;

	explicit Task( const QString& exe ) ;
	Task( const QString&,const QString& ) ;
	Task( lxqt::Wallet::Wallet *,QVector<lxqt::Wallet::walletKeyValues> * ) ;
	Task( lxqt::Wallet::Wallet *,const QString& volumeID ) ;
	Task( lxqt::Wallet::Wallet *,const QString& volumeID,const QString& key,const QString& comment ) ;
	explicit Task( QTableWidget * ) ;
	Task() ;
	~Task() ;
	void start( Task::action = Task::exeTask ) ;
signals:
	void partitionProperties( QStringList ) ;
	void addItemToTable( QString,QString,QString ) ;
	void finished( QString ) ;
	void finished( void ) ;
	void finished( int ) ;
	void finished( int,QString ) ;
	void taskResult( QTableWidgetItem *,int ) ;
	void errorStatus( int exitCode,int exitStatus,int startError ) ;
private:
	void openMountPointTask( void ) ;
	void updateVolumeListTask( void ) ;
	void run( void ) ;
	void runExeTask( void ) ;
	void runCloseAllVolumeTask( void ) ;
	void runVolumePropertiesTask( void ) ;
	void runVolumeTask( void ) ;
	void addKeyTask( void ) ;
	void deleteKeyTask( void ) ;
	void getAllKeysTask( void ) ;
	QString m_exe ;
	QString m_output ;
	int m_status ;
	QTableWidget * m_table ;
	Task::action m_action ;
	QString m_path ;
	QString m_mpoint ;
	QString m_volumeProperties ;
	QString m_folderOpener ;
	QString m_partitionType ;
	lxqt::Wallet::Wallet * m_wallet ;
	QVector<lxqt::Wallet::walletKeyValues> * m_keys ;
	QString m_volumeID ;
	QString m_key ;
	QString m_comment ;
	int m_exitCode ;
	int m_exitStatus ;
	int m_startError ;
};

#endif // RUNINTHREAD_H

