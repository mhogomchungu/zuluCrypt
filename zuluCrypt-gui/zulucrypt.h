/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
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

#ifndef ZULUCRYPT_H
#define ZULUCRYPT_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QPoint>

#include "../zuluMount-gui/monitor_mountinfo.h"
#include "lxqt_wallet.h"
#include "utility.h"
#include "secrets.h"
#include "systemsignalhandler.h"
#include "debugwindow.h"

class QWidget ;
class QTableWidgetItem ;
class QDragEnterEvent ;
class QDropEvent ;
class QSystemTrayIcon ;
class QCloseEvent ;
class passwordDialog ;
class openvolume ;
class createvolume ;
class luksdeletekey ;
class luksaddkey ;
class managevolumeheader ;
class cryptfiles ;
class walletconfig ;
class QNetworkReply ;

/*
 * below header is created at build time,it is set by CMakeLists.txt located in the root folder
 */
#include "version.h"

namespace Ui {
    class zuluCrypt ;
}

class zuluCrypt : public QMainWindow
{
	Q_OBJECT
public:
	zuluCrypt( QWidget * parent = 0 ) ;
	~zuluCrypt() ;
signals:
	void closeVolume( QTableWidgetItem *,int ) ;
	void updateVolumeListSignal( QString,QString ) ;
private slots :
	void showLUKSSlotsData( void ) ;
	void showLUKSSlotsInfo( void ) ;
	void createVolumeInFile() ;
	void showTrayIcon( bool ) ;
	void polkitFailedWarning( void ) ;
	void setFileManager( void ) ;
	void helperStarted( bool,const QString& ) ;
	void start( void ) ;
	void showTrayGUI( void ) ;
	void autoOpenMountPoint( bool ) ;
	void languageMenu( QAction * ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void info( void ) ;
	void luksAddKeyContextMenu( void ) ;
	void luksDeleteKeyContextMenu( void ) ;
	void aboutMenuOption( void ) ;
	void close(  void  ) ;
	void closeAll( QTableWidgetItem *,int ) ;
	void itemClicked(  QTableWidgetItem * item ) ;
	void itemClicked( QTableWidgetItem * item,bool ) ;
	void itemClicked( QTableWidgetItem * item,QPoint point ) ;
	void volume_property( void ) ;
	void UIMessage( QString title,QString message ) ;
	void fonts( void ) ;
	void addToFavorite( void ) ;
	void readFavorites( void ) ;
	void favClicked( QAction * ) ;
	void favAboutToHide( void ) ;
	void trayClicked( QSystemTrayIcon::ActivationReason ) ;
	void trayProperty( void ) ;
	void closeApplication( void ) ;
	void closeApplication( int ) ;
	void minimizeToTray( void ) ;
	void closeAllVolumes( void ) ;
	void emergencyQuitApplication( void ) ;
	void menuKeyPressed( void ) ;
	void ShowCreateFile( void ) ;
	void ShowFavoritesEntries( void ) ;
	void ShowCreateKeyFile( void ) ;
	void ShowDeleteKeyContextMenu( QString ) ;
	void ShowDeleteKey( void ) ;
	void ShowAddKeyContextMenu( QString ) ;
	void ShowAddKey( void ) ;
	void ShowNonSystemPartitions( void ) ;
	void ShowPasswordDialog( void ) ;
	void ShowOpenPartition( void ) ;
	void ShowManageSystemPartitions( void ) ;
	void ShowManageNonSystemPartitions( void ) ;
	void ShowPasswordDialog( QString,QString ) ;
	void ShowEraseDataDialog( void ) ;
	void luksHeaderBackUpContextMenu( void ) ;
	void volumeHeaderBackUp( void ) ;
	void volumeRestoreHeader( void ) ;
	void permissionExplanation( void ) ;
	void encryptFile( void ) ;
	void decryptFile( void ) ;
	void HelpLuksHeaderBackUp( void ) ;
	void setDefaultWallet( void ) ;
	void failedToOpenWallet( void ) ;
	void openFolder( void ) ;
	void openSharedFolder( void ) ;
	void itemEntered( QTableWidgetItem * ) ;
	void optionMenuAboutToShow( void ) ;
	void openpdf( void ) ;
	void cinfo( void ) ;
	void updateVolumeList( const QString& = QString() ) ;
	void updateVolumeList( QString,QString ) ;
	void createVolumeInExistingFile( void ) ;
private:
	void raiseWindow( const QString& ) ;
	void setIcons( void ) ;
	void quitApplication( void ) ;
	void removeRowFromTable( int ) ;
	void openFolder( const QString& ) ;
	void dragEnterEvent( QDragEnterEvent * ) ;
	void dropEvent( QDropEvent * ) ;

	passwordDialog& setUpPasswordDialog( void ) ;

	void setLocalizationLanguage( bool ) ;
	void setupConnections( void ) ;
	void setupUIElements( void ) ;
	void closeEvent( QCloseEvent * ) ;
	void setUserFont( QFont ) ;
	void initTray( bool ) ;
	void initFont( void ) ;
	void initKeyCombo( void ) ;
	void closeStatusErrorMessage( int ) ;
	void decryptFile( const QString& ) ;
	void updateTrayContextMenu( void ) ;

	QMenu m_trayIconMenu ;

	secrets m_secrets ;

	Ui::zuluCrypt * m_ui = nullptr ;

	QMenu * m_language_menu = nullptr ;

	QSystemTrayIcon m_trayIcon ;

	QString m_sharedMountPoint ;

	QString m_openPath ;
	bool m_startHidden ;
	int m_userID ;
	bool m_autoOpenMountPoint ;
	monitor_mountinfo m_mountInfo ;
	systemSignalHandler m_signalHandler ;
	debugWindow m_debugWindow ;
};

#endif // ZULUCRYPT_H
