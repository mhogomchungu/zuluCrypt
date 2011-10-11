/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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


#include "executables.h"
#include "password_dialog.h"
#include "openpartition.h"
#include "luksaddkey.h"
#include "luksdeletekey.h"
#include "createpartition.h"
#include "createfile.h"
#include "createkeyfile.h"
#include "rngselector.h"

namespace Ui {
    class zuluCrypt;
}

class zuluCrypt : public QMainWindow
{
	Q_OBJECT

public:
	explicit zuluCrypt(QWidget *parent = 0);

	void removeRowFromTable(int row) ;

	static char luksEmptySlots(QString volumePath) ;

	static bool isLuks(QString volumePath) ;

	~zuluCrypt();

signals:
	void  showManageFavorites(void) ;
	void  favClickedVolume(QString volume_path,QString mount_point_path);
	void  showNonSystemPartitions(void);
	void  luksAddKeyUI(QString volumePath) ;
	void  luksDeleteKeyUI(QString passphrase) ;
	void  redoOpen(bool boolOpenReadOnly,bool boolKeyFromFile,QString volumePath, QString mountPointPath) ;

private slots :

	void info(void) ;

	void createEncryptedpartitionUI(void) ;

	void luksAddKeyContextMenu(void) ;

	void luksDeleteKeyContextMenu() ;

	void aboutMenuOption(void) ;

	void close(void) ;

	void options( QTableWidgetItem * item) ;

	void volume_property(void) ;

	void setUpOpenedVolumes(void) ;

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


private:

	void deleteFile(QFile *) ;

	void setUserFont(void) ;

	void setupUserOptions(void) ;

	Ui::zuluCrypt *ui;

	password_Dialog *openFileUI ;

	openpartition *openPartitionUI ;

	openpartition *NonSystemPartitions ;

	openpartition *luksopenPartitionUI ;

	createpartition *createpartitionUI ;

	luksaddkeyUI *addKeyUI ;

	luksdeletekey *deleteKeyUI ;

	createfile *createFile ;

	createkeyfile *createkeyFile ;

	QString volume_path,mount_point_path, pass_phrase,mode ;

	QMenu *m  ;

	QTableWidgetItem* item ;

	int item_count ;

	QSystemTrayIcon *trayIcon ;

	QFont Font ;

	QList<QMenu *> menulist ;

	rngselector *rng ;

	volumePropertiesThread *vpt ;

	QString volumeProperty ;

	QMessageBox *mp ;

};

#endif // ZULUCRYPT_H
