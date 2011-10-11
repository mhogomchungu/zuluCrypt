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
	ui = new Ui::PasswordDialog() ;
	ui->setupUi(this);
	this->setFixedSize(this->size());

	passphraseOption() ;

	connect(ui->PushButtonCancel,SIGNAL(clicked()),this,SLOT(HideUI())) ;
	connect(ui->PushButtonOpen,SIGNAL(clicked()),this,SLOT(buttonOpenClicked())) ;
	connect(ui->PushButtonMountPointPath,SIGNAL(clicked()),this,SLOT(mount_point()));
	connect(ui->PushButtonVolumePath,SIGNAL(clicked()),this,SLOT(file_path())) ;
	connect(ui->pushButtonPassPhraseFromFile,SIGNAL(clicked()),this,SLOT(clickedPassPhraseFromFileButton()));
	connect(ui->radioButtonPassPhraseFromFile,SIGNAL(clicked()),this,SLOT(passphraseFromFileOption())) ;
	connect(ui->radioButtonPassPhrase,SIGNAL(clicked()),this,SLOT(passphraseOption())) ;
}

void password_Dialog::ShowUI(QString volumePath, QString mount_point)
{
	ui->OpenVolumePath->setText(volumePath);
	ui->MountPointPath->setText(mount_point);
	ui->checkBoxReadOnly->setChecked( false );
	ui->PassPhraseField->setFocus();
	ui->radioButtonPassPhraseFromFile->setChecked( false );
	this->show();
}

void password_Dialog::ShowUI(bool A,bool B,QString C,QString D)
{
	ui->checkBoxReadOnly->setChecked( A );
	ui->radioButtonPassPhraseFromFile->setChecked( B );
	ui->OpenVolumePath->setText( C );
	ui->MountPointPath->setText( D );
	ui->PassPhraseField->setFocus();
	this->show();
}

void password_Dialog::passphraseOption()
{
	ui->PassPhraseField->setEchoMode(QLineEdit::Password);
	ui->PassPhraseField->clear();
	ui->pushButtonPassPhraseFromFile->setEnabled(false) ;
	ui->labelPassphrase->setText(QString("passphrase"));
}

void password_Dialog::passphraseFromFileOption()
{
	ui->PassPhraseField->setEchoMode(QLineEdit::Normal);
	ui->PassPhraseField->clear();
	ui->pushButtonPassPhraseFromFile->setEnabled(true) ;
	ui->labelPassphrase->setText(QString("key file"));
}

void password_Dialog::clickedPassPhraseFromFileButton()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("Select passphrase file"),QDir::homePath(),0);
	ui->PassPhraseField->setText( Z );
}

void password_Dialog::clickedPartitionOption(QString option)
{
	ui->OpenVolumePath->setText(option);
	ui->PassPhraseField->clear();
	ui->MountPointPath->setText(QDir::homePath());
	if( this->isHidden() )
		this->show();
}

void password_Dialog::mount_point(void )
{	
	QString Z = QFileDialog::getExistingDirectory((QWidget *) this,QString("Select Path to mount point folder"),QDir::homePath(),QFileDialog::ShowDirsOnly) ;
	ui->MountPointPath->setText( Z );
}

void password_Dialog::file_path(void )
{	
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("Select encrypted volume"),QDir::homePath(),0);
	ui->OpenVolumePath->setText( Z );
}

void password_Dialog::ShowUI()
{
	ui->OpenVolumePath->setFocus();
	ui->MountPointPath->setText(QDir::homePath());
	this->show();
}

void password_Dialog::HideUI()
{
	ui->MountPointPath->clear();
	ui->OpenVolumePath->clear();
	ui->PassPhraseField->clear();
	ui->radioButtonPassPhrase->setChecked(true);
	ui->PassPhraseField->setEchoMode(QLineEdit::Password);
	ui->checkBoxReadOnly->setChecked(false);
	this->hide();
}

void password_Dialog::buttonOpenClicked(void )
{	
	bool A = ui->checkBoxReadOnly->isChecked() ;
	bool B = ui->radioButtonPassPhraseFromFile->isChecked() ;
	volumePath = ui->OpenVolumePath->text() ;
	QString mountPointPath = ui->MountPointPath->text() ;
	QString passPhraseField = ui->PassPhraseField->text() ;

	QMessageBox m ;
	m.addButton(QMessageBox::Ok);
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	if(volumePath.isEmpty()){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("ERROR: volume path field is empty"));
		m.exec() ;
		return ;
	}
	if(mountPointPath.isEmpty()){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("ERROR: mount point path field is empty"));
		m.exec() ;
		return ;
	}
	if(passPhraseField.isEmpty()){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("passphrase field is empty"));

		if( B == true )
			m.setText(QString("ERROR: key file field is empty"));
		else
			m.setText(QString("ERROR: passphrase field is empty"));

		m.exec() ;
		return ;
	}	

	QString mode ;

	if ( A == true )
		mode = "ro" ;
	else
		mode = "rw" ;

	QString passtype ;

	if ( B == true ){

		passtype = " -f " ;

	}else{
		passtype = " -p " ;

		for( int i = 0 ; i < passPhraseField.size() ; i++){

			if( passPhraseField.at(i).toAscii() == '\"'){
				passPhraseField.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	QString exe = QString(ZULUCRYPTzuluCrypt) + " open \"" + volumePath + "\" \"" + mountPointPath + "\" " + mode + " " + passtype + "\"" + passPhraseField +"\"";

	ovt = new openVolumeThread(exe,&status) ;

	connect(ovt,SIGNAL(finished()),this,SLOT(threadfinished())) ;

	disableAll();

	ovt->start();
}

void password_Dialog::disableAll()
{
	ui->checkBoxReadOnly->setEnabled(false);
	ui->groupBox->setEnabled(false);
	ui->labelMoutPointPath->setEnabled(false);
	ui->labelPassphrase->setEnabled(false);
	ui->labelVolumePath->setEnabled(false);
	ui->MountPointPath->setEnabled(false);
	ui->OpenVolumePath->setEnabled(false);
	ui->PassPhraseField->setEnabled(false);
	ui->PushButtonCancel->setEnabled(false);
	ui->PushButtonMountPointPath->setEnabled(false);
	ui->PushButtonOpen->setEnabled(false);
	ui->pushButtonPassPhraseFromFile->setEnabled(false);
	ui->PushButtonVolumePath->setEnabled(false);
	ui->radioButtonPassPhrase->setEnabled(false);
	ui->radioButtonPassPhraseFromFile->setEnabled(false);
	ui->radioButtonPassPhrase->setEnabled(false);
}

void password_Dialog::enableAll()
{
	ui->checkBoxReadOnly->setEnabled(true);
	ui->groupBox->setEnabled(true);
	ui->labelMoutPointPath->setEnabled(true);
	ui->labelPassphrase->setEnabled(true);
	ui->labelVolumePath->setEnabled(true);
	ui->MountPointPath->setEnabled(true);
	ui->OpenVolumePath->setEnabled(true);
	ui->PassPhraseField->setEnabled(true);
	ui->PushButtonCancel->setEnabled(true);
	ui->PushButtonMountPointPath->setEnabled(true);
	ui->PushButtonOpen->setEnabled(true);
	ui->pushButtonPassPhraseFromFile->setEnabled(true);
	ui->PushButtonVolumePath->setEnabled(true);
	ui->radioButtonPassPhrase->setEnabled(true);
	ui->radioButtonPassPhraseFromFile->setEnabled(true);
	ui->radioButtonPassPhrase->setEnabled(true);
}
void password_Dialog::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.exec() ;
}
void password_Dialog::threadfinished()
{
	delete ovt ;

	enableAll();

	switch ( status ){
		case 0 :{
			/*
			  There are possible names zuluCrypt-cli will use for mount point and predicting it before hand may
			  cause unnecessary code bloat. If the opening succeed, just go read the output of "mount"
			  and use whatever you will find.
			  */
			char * c = 0 ;

			char *d = 0 ;
			int k ;
			QString N ;
			QProcess Z ;
			Z.start(QString(ZULUCRYPTmount));

			Z.waitForFinished() ;

			c = Z.readAllStandardOutput().data() ;

			N = "/dev/mapper/zuluCrypt-" + volumePath.split("/").last() ;

			d = N.toAscii().data() ;

			k = strlen( d ) ;

			d = c = strstr( c , d )  + k + 4 ;

			while (*++d != ' ') { ; }

			*d = '\0' ;

			Z.close();			

			emit addItemToTable(volumePath,QString( c ));

			HideUI() ;

			}break ;

		case 1 : UIMessage(QString("ERROR"),QString("No free loop device to use.")) ;
			HideUI();
			return ;
			break ;

		case 2 : UIMessage(QString("ERROR"),QString("there seem to be an open volume accociated with given path."));
			break ;

		case 3 : UIMessage(QString("ERROR"),QString("no file or device on a given address.")) ;
			break ;

		case 4 :{
			UIMessage(QString("ERROR"),QString("wrong passphrase."));
			ui->PassPhraseField->clear();
			ui->PassPhraseField->setFocus();
			}break ;

		case 5 : UIMessage(QString("ERROR"),QString("mount point address is already taken by a file or folder")) ;
			break ;
		case 6 : UIMessage(QString("ERROR"),QString("passphrase file does not exist"));
			break ;
		case 9 : UIMessage(QString("ERROR"),QString("\",\" (comma) is not a valid mount point"));
			break ;
		default :UIMessage(QString("ERROR"),QString("un unknown error has occured, volume not opened"));
		}

}

password_Dialog::~password_Dialog()
{
	delete ui ;
}

