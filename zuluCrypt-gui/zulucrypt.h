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
	void start() ;
signals:
	void closeVolume( QTableWidgetItem *,int ) ;
	void updateUi( QVector< QStringList > ) ;
private:
	void updateUi0( QVector< QStringList > ) ;
	void helperStarted( bool,const QString& ) ;
	void showLUKSSlotsData() ;
	void showLUKSSlotsInfo() ;
	void createVolumeInFile() ;
	void showTrayIcon( bool ) ;
	void polkitFailedWarning() ;
	void setFileManager() ;
	void showTrayGUI() ;
	void autoOpenMountPoint( bool ) ;
	void languageMenu( QAction * ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void info() ;
	void luksAddKeyContextMenu() ;
	void luksDeleteKeyContextMenu() ;
	void aboutMenuOption() ;
	void close() ;
	void closeAll( QTableWidgetItem *,int ) ;
	void itemClicked0(  QTableWidgetItem * item ) ;
	void itemClicked( QTableWidgetItem * item,bool ) ;
	void itemClicked( QTableWidgetItem * item,QPoint point ) ;
	void volume_property() ;
	void UIMessage( QString title,QString message ) ;
	void fonts() ;
	void addToFavorite() ;
	void readFavorites() ;
	void favClicked( QAction * ) ;
	void favAboutToHide() ;
	void trayClicked( QSystemTrayIcon::ActivationReason ) ;
	void trayProperty() ;
	void closeApplication0() ;
	void closeApplication( int ) ;
	void minimizeToTray() ;
	void closeAllVolumes() ;
	void emergencyQuitApplication() ;
	void menuKeyPressed() ;
	void ShowCreateFile() ;
	void ShowFavoritesEntries() ;
	void ShowCreateKeyFile() ;
	void ShowDeleteKeyContextMenu( QString ) ;
	void ShowDeleteKey() ;
	void ShowAddKeyContextMenu( QString ) ;
	void ShowAddKey() ;
	void ShowNonSystemPartitions() ;
	void ShowPasswordDialog0() ;
	void ShowOpenPartition() ;
	void ShowManageSystemPartitions() ;
	void ShowManageNonSystemPartitions() ;
	void ShowPasswordDialog( QString,QString ) ;
	void ShowEraseDataDialog() ;
	void luksHeaderBackUpContextMenu() ;
	void volumeHeaderBackUp() ;
	void volumeRestoreHeader() ;
	void permissionExplanation() ;
	void encryptFile() ;
	void decryptFile0() ;
	void HelpLuksHeaderBackUp() ;
	void setDefaultWallet() ;
	void failedToOpenWallet() ;
	void openFolder() ;
	void openSharedFolder() ;
	void itemEntered( QTableWidgetItem * ) ;
	void optionMenuAboutToShow() ;
	void openpdf() ;
	void cinfo() ;
	void updateVolumeList1() ;
	void updateVolumeList( const QString& ) ;
	void updateVolumeList0( const QString& ) ;
	void createVolumeInExistingFile() ;
	void raiseWindow( const QString& ) ;
	void setIcons() ;
	void quitApplication() ;
	void removeRowFromTable( int ) ;
	void openFolder( const QString& ) ;
	void dragEnterEvent( QDragEnterEvent * ) ;
	void dropEvent( QDropEvent * ) ;

	passwordDialog& setUpPasswordDialog() ;

	void setLocalizationLanguage( bool ) ;
	void setupConnections() ;
	void setupUIElements() ;
	void closeEvent( QCloseEvent * ) ;
	void setUserFont( QFont ) ;
	void initTray( bool ) ;
	void initFont() ;
	void initKeyCombo() ;
	void closeStatusErrorMessage( int ) ;
	void decryptFile( const QString& ) ;
	void updateTrayContextMenu() ;

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
