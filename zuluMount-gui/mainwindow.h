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
#include <QFileDialog>
#include <QUrl>
#include <QTranslator>

#include <unistd.h>
#include "keydialog.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/userfont.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "mountpartition.h"
#include "../zuluCrypt-gui/openvolumereadonly.h"
#include "oneinstance.h"
#include "auto_mount.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	explicit MainWindow( int argc,char * argv[],QWidget * parent = 0 );
	~MainWindow();
	void start( void ) ;
	static void volumeMiniProperties( QTableWidget *,QString,QString );
signals:
	void result( int,QString ) ;
public slots:
	void raiseWindow( void ) ;
	void raiseWindow( QString ) ;
private slots:
	void showMoungDialog( QStringList );
	void autoMountVolumeSystemInfo( QStringList ) ;
	void autoMountVolumeInfo( QStringList ) ;
	void mount( QString,QString,QString ) ;
	void defaultButton( void ) ;
	void volumeProperties( QString );
	void volumeProperties( void ) ;
	void itemClicked( QTableWidgetItem * ) ;
	void pbUpdate( void ) ;
	void pbMount( void ) ;
	void slotMount( void ) ;
	void pbUmount( void ) ;
	void pbClose( void ) ;
	void slotUnmountComplete( int,QString,QString ) ;
	void slotMountedList( QStringList,QStringList ) ;
	void slotcbReadOnly( void ) ;
	void slotCloseApplication( void ) ;
	void slotTrayClicked( QSystemTrayIcon::ActivationReason ) ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll( void ) ;
	void stateChanged( int ) ;
	void slotOpenFolder( void ) ;
	void slotOpenSharedFolder( void ) ;
	void fileManagerOpenStatus( int exitCode, int exitStatus,int startError ) ;
	void processArgumentList( void ) ;
	void openVolumeFromArgumentList( void ) ;
	void itemEntered( QTableWidgetItem * ) ;
	void deviceRemoved( QString ) ;
	void autoMountAddToTable( QString ) ;
	void addEntryToTable( bool,QStringList ) ;
	void started( void ) ;
	void close( void ) ;
	void autoMountToggled( bool ) ;
	void autoOpenFolderOnMount( bool ) ;
private:
	void setLocalizationLanguage( void ) ;
	bool autoOpenFolderOnMount( void ) ;
	void dragEnterEvent( QDragEnterEvent * ) ;
	void dropEvent( QDropEvent * ) ;
	void showContextMenu( QTableWidgetItem *,bool ) ;
	void startAutoMonitor( void ) ;
	void showEvent( QShowEvent * );
	bool autoMount( void ) ;
	Ui::MainWindow * m_ui ;

	QString m_action ;
	QString m_device ;
	QString m_folderOpener ;
	int m_argc ;
	char ** m_argv ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * e );
	void setUpFont( void ) ;
	void setUpShortCuts( void ) ;
	void setUpApp( void ) ;
	QSystemTrayIcon * m_trayIcon ;
	auto_mount * m_autoMountThread ;
	QAction * m_autoMountAction ;
	bool m_startHidden ;
	bool m_started ;
	bool m_autoMount ;
	QString m_sharedFolderPath ;
	bool m_autoOpenFolderOnMount ;
};

#endif // MAINWINDOW_H
