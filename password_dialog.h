/*

    Copyright (c) 2011
    name : mhogo mchungu 
    email: mhogomchungu@gmail.com
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef PASSWORD_DIALOG_H
#define PASSWORD_DIALOG_H


#include <QString>
#include <QFileDialog>
#include <QStringList>

#include "ui_password.h"
#include "openpartition.h"

class password_Dialog :  public QDialog
{	
	Q_OBJECT
public:	
	
	password_Dialog(QWidget *parent = 0);
	virtual ~password_Dialog();    
	
	QString volume_path ;
	
	QString mount_point_path ;
	
	QString passphrase ;
	
	QString mode ;		

	bool boolPassphraseFromFile ;

	void clearAllFields(void) ;

	Ui::PasswordDialog passphraseDialogUI;

public slots :
	void buttomOpenClicked(void) ;
	void showDialog(void);
	void hideDialog(void) ;
	void mount_point(void);
	void file_path(void);
	void clickedPartitionOption(QString);
	void clickedPassPhraseFromFileButton(void) ;
	void passphraseFromFileOption(void) ;
	void passphraseOption(void) ;
	
private :
	openpartition p ;
};

#endif // PASSWORD_DIALOG_H

