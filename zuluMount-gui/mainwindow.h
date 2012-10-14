#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

#include <QString>
#include <QStringList>
#include <QTableWidgetItem>
#include <QDir>
#include <QIcon>
#include <QAction>
#include <QKeySequence>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QFont>
#include <QCursor>
#include <QAction>

#include "keydialog.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/userfont.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "mountpartition.h"
#include "../zuluCrypt-gui/openvolumereadonly.h"
#include "oneinstance.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	explicit MainWindow( QWidget * parent = 0 );
	~MainWindow();
	void start( void ) ;
	static void volumeMiniProperties( QTableWidget *,QString,QString );
	static void saveMountPointPath( QString ) ;
	static QString getMountPointPath( QString ) ;
signals:
	void result( int,QString ) ;
public slots:
	void raiseWindow( void ) ;
private slots:
	void defaultButton( void ) ;
	void volumeProperties( QString );
	void volumeProperties( void ) ;
	void itemClicked( QTableWidgetItem * ) ;
	void pbUpdate( void ) ;
	void pbMount( void ) ;
	void pbUmount( void ) ;
	void pbClose( void ) ;
	void slotUnmountComplete( int,QString ) ;
	void slotMountedList( QStringList,QStringList ) ;
	void slotcbReadOnly( void ) ;
	void slotCloseApplication( void ) ;
	void slotTrayClicked( QSystemTrayIcon::ActivationReason ) ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll( void ) ;
	void stateChanged( int ) ;
private:
	Ui::MainWindow * m_ui ;

	QString m_action ;
	QString m_device ;

	bool m_working ;
	void disableCommand( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * e );
	void setUpFont( void ) ;
	void setUpShortCuts( void ) ;
	void setUpApp( void ) ;
	bool m_justMounted ;
	QSystemTrayIcon * m_trayIcon ;
};

#endif // MAINWINDOW_H
