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
	void  luksAddKeyUI(QString volumePath) ;
	void  luksDeleteKeyUI(QString passphrase) ;
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
	void itemClicked(QTableWidgetItem *item,bool);
	void volume_property(void) ;
	void UIMessage(QString title,QString message) ;
	void fonts(void) ;
	void addToFavorite(void) ;
	void readFavorites(void) ;
	void favClicked(QAction *e) ;
	void favAboutToHide(void) ;
	void trayClicked(QSystemTrayIcon::ActivationReason e) ;
	void trayProperty(void) ;
	void addItemToTable(QString pathToVolume, QString pathToMountPoint);
	void volumePropertyThreadFinished(void) ;
	void closeApplication(void) ;
	void sovfinished(void) ;
	void minimize(void) ;
	void minimizeToTray(void);
	void closeAllVolumes(void) ;
	void deleteThread(closeAllVolumesThread*) ;
	void closeThreadFinished(runInThread *,int) ;
	void deleteAddItemToTableThread(addItemToTableThread *);
	void menuKeyPressed(void) ;
	void addItemToTableByVolume(QString volume_path);
	void ShowCreateFileUI(void);
	void FileCreated(QString);
	void HideCreateFileUI(createfile *);
	void showManageFavoritesUI(void);
	void HideManageFavoritesUI(managedevicenames * );
	void ShowCreateKeyFileUI(void);
	void HideCreateKeyFileUI(createkeyfile *);
	void ShowDeleteKeyContextMenu(QString);
	void ShowDeleteKeyUI(void);
	void HideDeleteKeyUI(luksdeletekey *);
	void ShowAddKeyContextMenu(QString);
	void ShowAddKeyUI(void);
	void HideAddKeyUI(luksaddkeyUI *);
	void HideCreatePartitionUI(createpartition *);
	void ShowNonSystemPartitions(void);
	void HideNonSystemPartitionUI(openpartition * );
	void ShowPasswordDialogUI(void);
	void HidePasswordDialogUI(password_Dialog *);
	void ShowOpenPartitionUI(void);
	void HideOpenPartitionUI(openpartition *);
	void ShowPasswordDialogUIFromFavorite(QString,QString);
	void volumeOpened(QString,QString,password_Dialog *);
	void partitionClicked(QString);
private:
	password_Dialog *setUpPasswordDialog(void);
	void setupConnections(void) ;
	void setupUIElements(void) ;
	void closeEvent(QCloseEvent *) ;
	void setUserFont(QFont) ;
	void HighlightRow(int,int) ;
	void StartUpAddOpenedVolumesToTable(void);
	bool checkUUID(QString *uuid,QString entry);

	Ui::zuluCrypt *ui;
	QTableWidgetItem* current_table_item ;
	volumePropertiesThread *vpt ;
	QString volumeProperty ;	
	QSystemTrayIcon *trayIcon ;
	QAction * rca;
};

#endif // ZULUCRYPT_H
