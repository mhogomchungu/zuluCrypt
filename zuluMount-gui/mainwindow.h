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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QVector>

#include "volumeentryproperties.h"

class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;
class events ;
class monitor_mountinfo ;

namespace Ui {
class MainWindow ;
}

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	explicit MainWindow( int argc,char * argv[],QWidget * parent = 0 ) ;
	~MainWindow() ;
	void start( void ) ;
signals:
	void result( int,QString ) ;
	void unlistVolume( QString ) ;
public slots:
	void raiseWindow( void ) ;
	void raiseWindow( QString ) ;
private slots:
	void startGUI( void ) ;
	void volumeMiniProperties( volumeEntryProperties * ) ;
	void showMoungDialog( volumeEntryProperties * ) ;
	void autoMountVolume( volumeEntryProperties * ) ;
	void mount( const volumeEntryProperties& ) ;
	void defaultButton( void ) ;
	void volumeProperties( QString ) ;
	void volumeProperties( void ) ;
	void itemClicked( QTableWidgetItem * ) ;
	void pbUpdate( void ) ;
	void pbMount( void ) ;
	void slotMount( void ) ;
	void unMountAll( void ) ;
	void pbUmount( void ) ;
	void pbClose( void ) ;
	void slotUnmountComplete( int,QString ) ;
	void volumeList( QVector< volumeEntryProperties > * ) ;
	void slotTrayClicked( QSystemTrayIcon::ActivationReason ) ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll( void ) ;
	void enableAll_1( void ) ;
	void slotOpenFolder( void ) ;
	void slotOpenSharedFolder( void ) ;
	void fileManagerOpenStatus( int exitCode, int exitStatus,int startError ) ;
	void processArgumentList( void ) ;
	void openVolumeFromArgumentList( void ) ;
	void itemEntered( QTableWidgetItem * ) ;
	void volumeRemoved( QString ) ;
	void removeVolume( QString ) ;
	void addEntryToTable( bool,const QStringList& ) ;
	void quitApplication( void ) ;
	void autoMountToggled( bool ) ;
	void autoOpenFolderOnMount( bool ) ;
	void removeEntryFromTable( QString ) ;
	void showFavorites( void ) ;
	void favoriteClicked( QAction * ) ;
private:
	QFont getSystemVolumeFont( void ) ;
	void setLocalizationLanguage( void ) ;
	bool autoOpenFolderOnMount( void ) ;
	void dragEnterEvent( QDragEnterEvent * ) ;
	void removeDisappearedEntries( const QVector< volumeEntryProperties >& ) ;
	void dropEvent( QDropEvent * ) ;
	void showContextMenu( QTableWidgetItem *,bool ) ;
	void startAutoMonitor( void ) ;
	bool autoMount( void ) ;
	void errorReadingList( void ) ;
	void updateList( const volumeEntryProperties& ) ;

	Ui::MainWindow * m_ui ;

	QString m_action ;
	QString m_device ;
	QString m_folderOpener ;
	int m_argc ;
	char ** m_argv ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * e ) ;
	void setUpFont( void ) ;
	void setUpShortCuts( void ) ;
	void setUpApp( void ) ;
	QSystemTrayIcon * m_trayIcon ;
	events * m_events ;
	monitor_mountinfo * m_mountInfo ;
	QAction * m_autoMountAction ;
	QMenu * m_favorite_menu ;
	bool m_startHidden ;
	bool m_autoMount ;
	QString m_sharedFolderPath ;
	bool m_autoOpenFolderOnMount ;
	bool m_removeAllVolumes ;
};

#endif // MAINWINDOW_H
