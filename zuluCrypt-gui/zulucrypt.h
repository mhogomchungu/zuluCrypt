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
#include <QTableWidgetItem>
#include <QSystemTrayIcon>
#include <QFont>
#include <QWidget>
#include <QList>
#include <QMessageBox>
#include <QCloseEvent>

#include "../zuluCrypt-cli/constants.h"
#include "../zuluMount-gui/oneinstance.h"
#include "password_dialog.h"
#include "openpartition.h"
#include "luksaddkey.h"
#include "luksdeletekey.h"
#include "createpartition.h"
#include "createfile.h"
#include "createkeyfile.h"
#include "startupupdateopenedvolumes.h"
#include "closeallvolumesthread.h"
#include "runinthread.h"
#include "managedevicenames.h"
#include "volumepropertiesthread.h"
#include "closevolumethread.h"
#include "checkvolumetype.h"
#include "cryptoinfo.h"
#include "erasedevice.h"
#include "manageluksheader.h"
#include "cryptfiles.h"
#include "dialogmsg.h"
#include "managesystemvolumes.h"
#include "userfont.h"
#include "kwalletplugin.h"
#include "kwalletconfig.h"
#include "tablewidget.h"
#include "openmountpointinfilemanager.h"
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
	void volumePropertyThreadFinished( QString properties ) ;
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
	void ShowPasswordDialogFromFavorite( QString,QString );
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
	void manageWallet( void ) ;
	void setDefaultWallet( void ) ;
	void failedToOpenWallet( void ) ;
	void openFolder( void ) ;
private:
	passwordDialog * setUpPasswordDialog( void );
	openpartition * setUpOpenpartition( void );
	createpartition * setUpCreatepartition( void );
	luksdeletekey * setUpluksdeletekey( void );
	luksaddkey * setUpluksaddkey( void );
	manageluksheader * setUpManageLuksHeader( void );
	cryptfiles * setUpCryptFiles( void );

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
	Ui::zuluCrypt * m_ui;
	QSystemTrayIcon * m_trayIcon ;
};

#endif // ZULUCRYPT_H
