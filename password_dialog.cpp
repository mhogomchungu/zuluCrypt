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


#include "password_dialog.h"
#include "zulucrypt.h"

#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>

password_Dialog::password_Dialog(QWidget *parent ) : QDialog(parent)
{
	ui.setupUi(this);
	this->setFixedSize(this->size());

	passphraseOption() ;

	connect(ui.PushButtonCancel,SIGNAL(clicked()),this,SLOT(HideUI())) ;
	connect(ui.PushButtonOpen,SIGNAL(clicked()),this,SLOT(buttonOpenClicked())) ;
	connect(ui.PushButtonMountPointPath,SIGNAL(clicked()),this,SLOT(mount_point()));
	connect(ui.PushButtonVolumePath,SIGNAL(clicked()),this,SLOT(file_path())) ;
	connect(ui.pushButtonPassPhraseFromFile,SIGNAL(clicked()),this,SLOT(clickedPassPhraseFromFileButton()));
	connect(ui.radioButtonPassPhraseFromFile,SIGNAL(clicked()),this,SLOT(passphraseFromFileOption())) ;
	connect(ui.radioButtonPassPhrase,SIGNAL(clicked()),this,SLOT(passphraseOption())) ;

}

void password_Dialog::ShowUI(bool A,bool B,QString C,QString D)
{
	ui.checkBoxReadOnly->setChecked( A );
	ui.radioButtonPassPhraseFromFile->setChecked( B );
	ui.OpenVolumePath->setText( C );
	ui.MountPointPath->setText( D );
	ShowUI();
}

void password_Dialog::passphraseOption()
{
	ui.PassPhraseField->setEchoMode(QLineEdit::Password);
	ui.PassPhraseField->clear();
	ui.pushButtonPassPhraseFromFile->setEnabled(false) ;
	ui.labelPassphrase->setText(QString("passphrase"));
}

void password_Dialog::passphraseFromFileOption()
{
	ui.PassPhraseField->setEchoMode(QLineEdit::Normal);
	ui.PassPhraseField->clear();
	ui.pushButtonPassPhraseFromFile->setEnabled(true) ;
	ui.labelPassphrase->setText(QString("key file"));
}

void password_Dialog::clickedPassPhraseFromFileButton()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("Select passphrase file"),QDir::homePath(),0);
	ui.PassPhraseField->setText( Z );
}

void password_Dialog::clickedPartitionOption(QString option)
{
	ui.OpenVolumePath->setText(option);
	ui.MountPointPath->clear();
	ui.PassPhraseField->clear();
	ui.MountPointPath->setFocus();

	if( this->isHidden() )
		this->show();
}

void password_Dialog::mount_point(void )
{	
	QString Z = QFileDialog::getExistingDirectory((QWidget *) this,QString("Select Path to mount point folder"),QDir::homePath(),QFileDialog::ShowDirsOnly) ;
	ui.MountPointPath->setText( Z );
}

void password_Dialog::file_path(void )
{	
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("Select encrypted volume"),QDir::homePath(),0);
	ui.OpenVolumePath->setText( Z );
}

void password_Dialog::ShowUI()
{
	ui.OpenVolumePath->setFocus();
	this->show();
}

void password_Dialog::HideUI()
{
	ui.MountPointPath->clear();
	ui.OpenVolumePath->clear();
	ui.PassPhraseField->clear();
	ui.radioButtonPassPhrase->setChecked(true);
	ui.PassPhraseField->setEchoMode(QLineEdit::Password);
	ui.checkBoxReadOnly->setChecked(false);
	this->hide();
}

void password_Dialog::buttonOpenClicked(void )
{	
	bool A = ui.checkBoxReadOnly->isChecked() ;
	bool B = ui.radioButtonPassPhraseFromFile->isChecked() ;
	QString C = ui.OpenVolumePath->text() ;
	QString D = ui.MountPointPath->text() ;
	QString E = ui.PassPhraseField->text() ;

	QMessageBox m ;
	m.addButton(QMessageBox::Ok);
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	if(C.isEmpty()){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("ERROR: volume path field is empty"));
		m.exec() ;
		return ;
	}
	if(D.isEmpty()){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("ERROR: mount point path field is empty"));
		m.exec() ;
		return ;
	}
	if(E.isEmpty()){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("ERROR:"));

		if( B == true )
			m.setText(QString("ERROR: key file field is empty"));
		else
			m.setText(QString("ERROR: passphrase field is empty"));

		m.exec() ;
		return ;
	}
	HideUI() ;
	emit pbOpenClicked(A,B,C,D,E);
}

password_Dialog::~password_Dialog()
{

}

