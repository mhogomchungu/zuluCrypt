/*
 * 
 *  Copyright (c) 2011
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
#include <QMutex>

#include "../zuluCrypt-cli/executables.h"
#include "../zuluCrypt-cli/version.h"
#include "password_dialog.h"
#include "openpartition.h"
#include "luksaddkey.h"
#include "luksdeletekey.h"
#include "createpartition.h"
#include "createfile.h"
#include "createkeyfile.h"
#include "startupupdateopenedvolumes.h"
#include "closeallvolumesthread.h"
#include "zulucryptthreads.h"
#include "additemtotablethread.h"
#include "runinthread.h"
#include "managedevicenames.h"

namespace Ui {
    class zuluCrypt;
}

class zuluCrypt : public QMainWindow
{
	Q_OBJECT
public:
	explicit zuluCrypt(QWidget *parent = 0);
	void removeRowFromTable(int row) ;
	~zuluCrypt();
signals:
	void  showManageFavorites(void) ;
	void  favClickedVolume(QString volume_path,QString mount_point_path);
	void  SignalShowNonSystemPartitions(void);
	void  luksAddKey(QString volumePath) ;
	void  luksDeleteKey(QString passphrase) ;
	void  redoOpen(bool boolOpenReadOnly,bool boolKeyFromFile,QString volumePath, QString mountPointPath) ;
private slots :
	void currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous );
	void info(void) ;
	void createEncryptedpartitionUI(void) ;
	void luksAddKeyContextMenu(void) ;
	void luksDeleteKeyContextMenu() ;
	void aboutMenuOption(void) ;
	void close( void ) ;
	void closeAll(QTableWidgetItem *,int) ;
	void itemClicked( QTableWidgetItem * item) ;
	void itemClicked(QTableWidgetItem * item,bool);
	void volume_property(void) ;
	void UIMessage(QString title,QString message) ;
	void fonts(void) ;
	void addToFavorite(void) ;
	void readFavorites(void) ;
	void favClicked(QAction *) ;
	void favAboutToHide(void) ;
	void trayClicked(QSystemTrayIcon::ActivationReason ) ;
	void trayProperty(void) ;
	void addItemToTable(QString pathToVolume, QString pathToMountPoint);
	void volumePropertyThreadFinished(void) ;
	void closeApplication(void) ;
	void StartUpAddOpenedVolumesToTableThreadFinished(startupupdateopenedvolumes *) ;
	void minimize(void) ;
	void minimizeToTray(void);
	void closeAllVolumes(void) ;
	void deletecloseAllVolumesThread(closeAllVolumesThread *) ;
	void closeThreadFinished(runInThread *,int) ;
	void deleteAddItemToTableThread(addItemToTableThread *);
	void menuKeyPressed(void) ;
	void addItemToTableByVolume(QString volume_path);
	void ShowCreateFile(void);
	void FileCreated(QString);
	void HideCreateFile(createfile *);
	void ShowFavoritesEntries(void);
	void HideManageFavorites(managedevicenames * );
	void ShowCreateKeyFile(void);
	void HideCreateKeyFile(createkeyfile *);
	void ShowDeleteKeyContextMenu(QString);
	void ShowDeleteKey(void);
	void HideDeleteKey(luksdeletekey *);
	void ShowAddKeyContextMenu(QString);
	void ShowAddKey(void);
	void HideAddKey(luksaddkey *);
	void HideCreatePartition(createpartition *);
	void ShowNonSystemPartitions(void);
	void HideNonSystemPartition(openpartition * );
	void ShowPasswordDialog(void);
	void HidePasswordDialog(passwordDialog *);
	void ShowOpenPartition(void);
	void HideOpenPartition(openpartition *);
	void ShowPasswordDialogFromFavorite(QString,QString);
	void volumeOpened(QString,QString,passwordDialog *);
	void partitionClicked(QString);
	void createPartition(QString);
private:
	passwordDialog * setUpPasswordDialog(void);
	openpartition * setUpOpenpartition(void);
	createpartition * setUpCreatepartition(void);
	luksdeletekey * setUpluksdeletekey(void);
	luksaddkey * setUpluksaddkey(void);

	void StartUpAddOpenedVolumesToTableThread(void);
	void setupConnections(void) ;
	void setupUIElements(void) ;
	void closeEvent(QCloseEvent *) ;
	void setUserFont(QFont) ;
	void HighlightRow(int,int) ;

	Ui::zuluCrypt * m_ui;
	QTableWidgetItem * m_currentItem ;
	volumePropertiesThread * m_vpt ;
	QString m_volumeProperty ;
	QSystemTrayIcon * m_trayIcon ;
	QAction * m_rca;
};

#endif // ZULUCRYPT_H
