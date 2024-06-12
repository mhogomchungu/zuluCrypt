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

#include "volumeproperty.h"
#include "../zuluCrypt-gui/secrets.h"
#include "events.h"
#include "monitor_mountinfo.h"
#include "../zuluCrypt-gui/systemsignalhandler.h"
#include "../zuluCrypt-gui/debugwindow.h"

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
	void Show() ;
	~zuluMount() ;
signals:
	void result( int,QString ) ;
	void unlistVolume( QString ) ;
private:
	void showTrayIcon( bool ) ;
	void polkitFailedWarning() ;
	void helperStarted( bool,const QString& ) ;
	void raiseWindow( const QString& = QString() ) ;
	void showTrayGUI() ;
	void cryfsVolumeProperties() ;
	void closeApplication() ;
	void closeApplication( int ) ;
	void unlockCryptFs() ;
	void showVisibleVolumeList() ;
	void showHiddenVolumeList() ;
	void removeVolumeFromHiddenVolumeList( QAction * ) ;
	void removeVolumeFromVisibleVolumeList( QAction * ) ;
	void volumeMiniProperties( bool,volumeProperty * ) ;
	void volumeMiniProperties( volumeProperty * ) ;
	void volumeMiniProperties_0( volumeProperty * ) ;
	void showMoungDialog( const volumeProperty& ) ;
	void showMoungDialog( const QString&,const QString& = QString() ) ;
	void autoMountVolume( volumeProperty * ) ;
	void mount( const volumeProperty& ) ;
	void defaultButton() ;
	void volumeProperties() ;
	void itemClicked( QTableWidgetItem * ) ;
	void pbUpdate() ;
	void pbMount() ;
	void slotMount() ;
	void unMountAll() ;
	void emergencyQuitApplication() ;
	void pbUmount() ;
	void pbUmount_powerDown() ;
	void unmount( const QString& = QString() ) ;
	void slotTrayClicked( QSystemTrayIcon::ActivationReason ) ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll() ;
	void enableAll_1() ;
	void slotOpenFolder() ;
	void slotOpenSharedFolder() ;
	void itemEntered( QTableWidgetItem * ) ;
	void volumeRemoved( QString ) ;
	void removeVolume( QString ) ;
	void addEntryToTable( bool,const QStringList& ) ;
	void addEntryToTable( bool,const volumeProperty& ) ;
	void autoMountToggled( bool ) ;
	void autoOpenFolderOnMount( bool ) ;
	void removeEntryFromTable( QString ) ;
	void showFavorites() ;
	void favoriteClicked( QAction * ) ;
	void openMountPointPath( QString ) ;
	void licenseInfo() ;
	void languageMenu( QAction * ac ) ;
	void encfsProperties() ;
	void securefsProperties() ;
	void gocryptfsProperties() ;
	void ecryptfsProperties() ;
private:
	void setIcons() ;
	bool errorNotFound( int ) ;
	QString resolveFavoriteMountPoint( const QString& ) ;
	void updateVolumeList( const QVector< volumeProperty >& ) ;
	void openMountPoint( const QString& ) ;
	QFont getSystemVolumeFont() ;
	void setLocalizationLanguage( bool ) ;
	bool autoOpenFolderOnMount() ;
	void dragEnterEvent( QDragEnterEvent * ) ;
	void removeDisappearedEntries( const QVector< volumeProperty >& ) ;
	void dropEvent( QDropEvent * ) ;
	void showContextMenu( QTableWidgetItem *,bool ) ;
	void startAutoMonitor() ;
	bool autoMount() ;
	void updateList( const volumeProperty& ) ;

	Ui::zuluMount * m_ui = nullptr ;
	QString m_folderOpener ;

	void disableAll() ;
	void closeEvent( QCloseEvent * e ) ;
	void setUpFont() ;
	void setUpShortCuts() ;
	void setUpApp( const QString& ) ;

	QAction * m_autoMountAction = nullptr ;
	QMenu * m_favorite_menu = nullptr ;
	QMenu * m_hidden_volume_menu = nullptr ;
	QMenu * m_not_hidden_volume_menu = nullptr ;
	QMenu * m_language_menu = nullptr ;

	secrets m_secrets ;

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

	monitor_mountinfo m_mountInfo ;
	events m_events ;

	systemSignalHandler m_signalHandler ;

	debugWindow m_debugWindow ;
};

#endif // MAINWINDOW_H
