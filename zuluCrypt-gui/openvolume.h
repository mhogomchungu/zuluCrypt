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

#ifndef OPEN_PARTITION_H
#define OPEN_PARTITION_H

#include<QString>
#include<QStringList>
#include<QDialog>

#include "utility.h"

class QWidget ;
class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;

namespace Ui {
class openvolume ;
}

class openvolume :  public QDialog
{
	Q_OBJECT
public:
	openvolume( QWidget * parent = 0 ) ;
	virtual ~openvolume() ;
	void showEncryptedOnly( void ) ;
	void showLuksOnly( void ) ;
signals :
	void HideUISignal( void ) ;
	void clickedPartition( QString ) ;
public slots:
	void tableEntryDoubleClicked( QTableWidgetItem * ) ;
	void ShowAllPartitions( void ) ;
	void HideUI( void ) ;
	void ShowNonSystemPartitions( void ) ;
	void ShowPartitionList( QString,QString ) ;
	void partitionList( QString,QString ) ;
	void allowLUKSOnly( void ) ;
private slots:
	void pbHelp( void ) ;
	void pbUUID( void ) ;
	void pbCancel( void ) ;
	void EnterKeyPressed( void ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void partitionProperties( const QStringList& ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::openvolume * m_ui ;
	QAction * m_action ;
	int m_option ;
	bool m_diableNonLUKS ;
	bool m_showEncryptedOnly ;
	bool m_showLuksOnly ;
};

#endif
