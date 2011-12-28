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

#ifndef PASSWORD_DIALOG_H
#define PASSWORD_DIALOG_H


#include <QString>
#include <QFileDialog>
#include <QStringList>
#include <QCloseEvent>

#include "ui_password.h"
#include "openpartition.h"
#include "zulucryptthreads.h"
#include "runinthread.h"

class password_Dialog :  public QDialog
{	
	Q_OBJECT
public:
	password_Dialog(QWidget *parent = 0);
	virtual ~password_Dialog();
signals :
	void addItemToTable(QString,QString) ;
	void HideUISignal(password_Dialog *);
	void volumeOpened(QString,QString,password_Dialog *);
public slots:
	void HideUI(void);
	void ShowUI(void);
	void ShowUI(QString volumePath,QString mount_point) ;
	void clickedPartitionOption(QString);
private slots :
	void buttonOpenClicked(void) ;
	void mount_point(void);	
	void clickedPassPhraseFromFileButton(void) ;
	void passphraseFromFileOption(void) ;
	void passphraseOption(void) ;	
	void file_path(void ) ;	
	void threadfinished(runInThread *,int) ;
	void mountPointPath(QString);
private :	
	void UIMessage(QString title, QString message) ;
	void disableAll(void);
	void enableAll(void) ;
	void closeEvent(QCloseEvent *) ;
	Ui::PasswordDialog *ui;
	QString volumePath ;
	QString m_point ;
	runInThread * ovt ;
};

#endif // PASSWORD_DIALOG_H

