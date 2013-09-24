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

#ifndef ZULUCRYPT_H
#define ZULUCRYPT_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QSystemTrayIcon>
#include "lxqt_wallet/frontend/lxqt_wallet.h"

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
class manageluksheader ;
class cryptfiles ;

/*
 * below header is created at build time,it is set by CMakeLists.txt located in the root folder
 */
#include "version.h"

namespace Ui {
    class zuluCrypt;
}

class zuluCrypt : public QMainWindow
{
	Q_OBJECT
public:
	zuluCrypt( QWidget * parent = 0 );
	void removeRowFromTable( int row ) ;
	void start( void ) ;
	~zuluCrypt();
public slots:
	void raiseWindow( void ) ;
	void raiseWindow( QString ) ;
signals:
	void showManageFavorites( void ) ;
	void favClickedVolume( QString volume_path,QString mount_point_path );
	void SignalShowNonSystemPartitions( void );
	void luksAddKey( QString volumePath ) ;
	void luksDeleteKey( QString passphrase ) ;
	void redoOpen( bool boolOpenReadOnly,bool boolKeyFromFile,QString volumePath,QString mountPointPath ) ;
private slots :
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous );
	void info( void ) ;
	void createEncryptedpartitionUI( void ) ;
	void luksAddKeyContextMenu( void ) ;
	void luksDeleteKeyContextMenu( void ) ;
	void aboutMenuOption( void ) ;
	void close(  void  ) ;
	void closeAll( QTableWidgetItem *,int ) ;
	void itemClicked(  QTableWidgetItem * item ) ;
	void itemClicked( QTableWidgetItem * item,bool );
	void volume_property( void ) ;
	void UIMessage( QString title,QString message ) ;
	void fonts( void ) ;
	void addToFavorite( void ) ;
	void readFavorites( void ) ;
	void favClicked( QAction * ) ;
	void favAboutToHide( void ) ;
	void trayClicked( QSystemTrayIcon::ActivationReason  ) ;
	void trayProperty( void ) ;
	void addItemToTable( QString pathToVolume,QString pathToMountPoint,QString volumeType );
	void volumePropertyTaskFinished( QString properties ) ;
	void closeApplication( void ) ;
	void startUpdateFinished( int ) ;
	void minimize( void ) ;
	void minimizeToTray( void );
	void closeAllVolumes( void ) ;
	void closeStatus( int ) ;
	void menuKeyPressed( void ) ;
	void ShowCreateFile( void );
	void FileCreated( QString );
	void ShowFavoritesEntries( void );
	void ShowCreateKeyFile( void );
	void ShowDeleteKeyContextMenu( QString );
	void ShowDeleteKey( void );
	void ShowAddKeyContextMenu( QString );
	void ShowAddKey( void );
	void ShowNonSystemPartitions( void );
	void ShowPasswordDialog( void );
	void ShowOpenPartition( void );
	void ShowManageSystemPartitions( void ) ;
	void ShowManageNonSystemPartitions( void ) ;
	void ShowPasswordDialog( QString,QString );
	void ShowEraseDataDialog( void );
	void partitionClicked( QString );
	void createPartition( QString );
	void luksHeaderBackUpContextMenu( void );
	void luksHeaderBackUp( void );
	void luksRestoreHeader( void );
	void permissionExplanation( void );
	void encryptFile( void );
	void decryptFile( void );
	void HelpLuksHeaderBackUp( void );
	void setDefaultWallet( void ) ;
	void failedToOpenWallet( void ) ;
	void openFolder( void ) ;
	void fileManagerOpenStatus( int exitCode, int exitStatus,int startError ) ;
	void openVolumeFromArgumentList( void ) ;
	void itemEntered( QTableWidgetItem * ) ;
	void manageVolumesInInternalWallet( void ) ;
	void manageVolumesInKDEWallet( void ) ;
	void manageVolumesInGNOMEWallet( void ) ;
	void changePassWordOfInternalWallet( void ) ;
	void walletpassWordChanged( bool ) ;
	void walletIsOpen( bool ) ;
	void optionMenuAboutToShow( void ) ;
private:
	void dragEnterEvent( QDragEnterEvent * );
	void dropEvent( QDropEvent * );
	passwordDialog * setUpPasswordDialog( void );
	openvolume * setUpOpenpartition( void );
	createvolume * setUpCreatepartition( void );
	luksdeletekey * setUpluksdeletekey( void );
	luksaddkey * setUpluksaddkey( void );
	manageluksheader * setUpManageLuksHeader( void );
	cryptfiles * setUpCryptFiles( void );

	void setLocalizationLanguage( void ) ;
	void StartUpAddOpenedVolumesToTableThread( void );
	void setupConnections( void ) ;
	void setupUIElements( void ) ;
	void closeEvent( QCloseEvent * ) ;
	void setUserFont( QFont ) ;
	void initTray( void );
	void initFont( void );
	void initKeyCombo( void );
	void closeStatusErrorMessage( int );
	void setUpApp( void ) ;
	void setFolderOpener( void ) ;
	void processArgumentList( void ) ;

	Ui::zuluCrypt * m_ui;
	QSystemTrayIcon * m_trayIcon ;
	QString m_folderOpener ;
	QString m_device ;
	lxqt::Wallet::Wallet * m_wallet ;
};

#endif // ZULUCRYPT_H
