/*
 *
 *  Copyright (c) 2012-2015
 *  name : Francis Banyikwa
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
class zuluMount ;
}

class zuluMount : public QWidget
{
	Q_OBJECT
public:
	explicit zuluMount( QWidget * parent = 0 ) ;
	~zuluMount() ;
	void Show( void ) ;
signals:
	void result( int,QString ) ;
	void unlistVolume( QString ) ;
public slots:
	void raiseWindow( void ) ;
	void raiseWindow( QString ) ;
private slots:
	void cryfsVolumeProperties( void ) ;
	void closeApplication( void ) ;
	void unlockCryptFs( void ) ;
	void showVisibleVolumeList( void ) ;
	void showHiddenVolumeList( void ) ;
	void removeVolumeFromHiddenVolumeList( QAction * ) ;
	void removeVolumeFromVisibleVolumeList( QAction * ) ;
	void startGUI( void ) ;
	void volumeMiniProperties( bool,volumeEntryProperties * ) ;
	void volumeMiniProperties( volumeEntryProperties * ) ;
	void volumeMiniProperties_0( volumeEntryProperties * ) ;
	void showMoungDialog( const volumeEntryProperties& ) ;
	void showMoungDialog( const QString&,const QString& = QString() ) ;
	void autoMountVolume( volumeEntryProperties * ) ;
	void mount( const volumeEntryProperties& ) ;
	void defaultButton( void ) ;
	void volumeProperties( void ) ;
	void itemClicked( QTableWidgetItem * ) ;
	void pbUpdate( void ) ;
	void pbMount( void ) ;
	void slotMount( void ) ;
	void unMountAll( void ) ;
	void pbUmount( void ) ;
	void pbUmount_powerDown( void ) ;
	void unmount( const QString& = QString() ) ;
	void slotTrayClicked( QSystemTrayIcon::ActivationReason ) ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll( void ) ;
	void enableAll_1( void ) ;
	void slotOpenFolder( void ) ;
	void slotOpenSharedFolder( void ) ;
	void itemEntered( QTableWidgetItem * ) ;
	void volumeRemoved( QString ) ;
	void removeVolume( QString ) ;
	void addEntryToTable( bool,const QStringList& ) ;
	void addEntryToTable( bool,const volumeEntryProperties& ) ;
	void quitApplication( void ) ;
	void autoMountToggled( bool ) ;
	void autoOpenFolderOnMount( bool ) ;
	void removeEntryFromTable( QString ) ;
	void showFavorites( void ) ;
	void favoriteClicked( QAction * ) ;
	void openMountPointPath( QString ) ;
	void licenseInfo( void ) ;
	void updateCheck( void ) ;
	void languageMenu( QAction * ac ) ;
private:
	void setIcons( void ) ;
	bool errorNotFound( int ) ;
	QString resolveFavoriteMountPoint( const QString& ) ;
	void autoUpdateCheck( void ) ;
	void updateVolumeList( const QVector< volumeEntryProperties >& ) ;
	void openMountPoint( const QString& ) ;
	QFont getSystemVolumeFont( void ) ;
	void setLocalizationLanguage( bool ) ;
	bool autoOpenFolderOnMount( void ) ;
	void dragEnterEvent( QDragEnterEvent * ) ;
	void removeDisappearedEntries( const QVector< volumeEntryProperties >& ) ;
	void dropEvent( QDropEvent * ) ;
	void showContextMenu( QTableWidgetItem *,bool ) ;
	void startAutoMonitor( void ) ;
	bool autoMount( void ) ;
	void updateList( const volumeEntryProperties& ) ;

	Ui::zuluMount * m_ui = nullptr ;
	QString m_folderOpener ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * e ) ;
	void setUpFont( void ) ;
	void setUpShortCuts( void ) ;
	void setUpApp( const QString& ) ;
	QAction * m_autoMountAction = nullptr ;
	QMenu * m_favorite_menu = nullptr ;
	QMenu * m_hidden_volume_menu = nullptr ;
	QMenu * m_not_hidden_volume_menu = nullptr ;
	QMenu * m_language_menu = nullptr ;

	bool m_startHidden ;
	bool m_autoMount ;
	QString m_sharedFolderPath ;
	bool m_autoOpenFolderOnMount ;
	bool m_removeAllVolumes = false ;
	QString m_env ;
	QString m_powerOff ;

	QVector< std::pair< QAction *,const char * > > m_actionPair ;
	QVector< std::pair< QMenu *,const char * > > m_menuPair ;

	QSystemTrayIcon m_trayIcon ;

	monitor_mountinfo& m_mountInfo ;
	events& m_events ;
};

#endif // MAINWINDOW_H
