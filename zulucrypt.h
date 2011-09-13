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

#include "password_dialog.h"
#include "openpartition.h"


namespace Ui {
    class zuluCrypt;
}

class zuluCrypt : public QMainWindow
{
	Q_OBJECT

public:
	explicit zuluCrypt(QWidget *parent = 0);

	void addItemToTable(QString pathToVolume, QString pathToMountPoint) ;

	void removeRowFromTable(int row) ;

	~zuluCrypt();

private slots :

	void aboutMenuOption(void) ;
	void showOpenFileDialogClear(void);

	void showOpenPartitionDialog(void) ;

	void open(void) ;

	void close(void) ;

	void options( QTableWidgetItem * item) ;

	void volume_property(void) ;

	void setUpOpenedVolumes(void) ;

	void UIMessage(QString message) ;

private:



	QString zuluCryptExe ;

	Ui::zuluCrypt *ui;
	password_Dialog openFileUI ;

	openpartition openPartitionUI ;

	QString volume_path,mount_point_path, pass_phrase,mode ;
	QMenu *m  ;
	QTableWidgetItem* item ;
	int item_count ;

};

#endif // ZULUCRYPT_H
