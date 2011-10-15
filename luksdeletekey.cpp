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

#include "luksdeletekey.h"
#include "zulucrypt.h"

#include <QObject>
#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>

namespace Ui {
    class luksdeletekey;
}

luksdeletekey::luksdeletekey(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::luksdeletekey)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());

	pUI = new openpartition(this);
	pUI->setWindowFlags(Qt::Window | Qt::Dialog);

	connect(pUI,SIGNAL(clickedPartition(QString)),this,SLOT(deleteKey(QString)));
	connect(this,SIGNAL(pbOpenPartitionClicked()),pUI,SLOT(ShowUI()));
	connect(ui->pushButtonDelete,SIGNAL(clicked()),this,SLOT(pbDelete())) ;
	connect(ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(ui->rbPassphrase,SIGNAL(toggled(bool)),this,SLOT(rbPassphrase())) ;
	connect(ui->rbPassphraseFromFile,SIGNAL(toggled(bool)),this, SLOT(rbPassphraseFromFile())) ;
	connect(ui->pushButtonOpenKeyFile,SIGNAL(clicked()),this,SLOT(pbOpenKeyFile())) ;
	connect(ui->pushButtonOpenVolume,SIGNAL(clicked()),this,SLOT(pbOpenVolume()));
	connect(ui->pushButtonOpenPartition,SIGNAL(clicked()),this,SLOT(pbOpenPartition())) ;
}

void luksdeletekey::rbPassphrase()
{
	ui->labelPassphrase->setText(QString("passphrase"));
	ui->lineEditPassphrase->setEchoMode(QLineEdit::Password);
	ui->lineEditPassphrase->clear();
	ui->pushButtonOpenKeyFile->setEnabled(false);
}

void luksdeletekey::rbPassphraseFromFile()
{
	ui->labelPassphrase->setText(QString("keyfile"));
	ui->lineEditPassphrase->setEchoMode(QLineEdit::Normal);
	ui->lineEditPassphrase->clear();
	ui->pushButtonOpenKeyFile->setEnabled(true);
}

void luksdeletekey::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("key file with a passphrase to delete"),QDir::homePath(),0);
	ui->lineEditPassphrase->setText( Z );
}

void luksdeletekey::ShowUI()
{
	enableAll();
	ui->rbPassphrase->setEnabled(true);
	ui->labelPassphrase->setText(QString("passphrase"));
	ui->rbPassphrase->setChecked(true);
	ui->lineEditVolumePath->setFocus();
	ui->pushButtonCancel->setDefault(true);
	this->show();
}

void luksdeletekey::disableAll()
{
	ui->groupBox->setEnabled(false);
	ui->label->setEnabled(false);
	ui->labelPassphrase->setEnabled(false);
	ui->lineEditPassphrase->setEnabled(false);
	ui->lineEditVolumePath->setEnabled(false);
	ui->pushButtonCancel->setEnabled(false);
	ui->pushButtonDelete->setEnabled(false);
	ui->pushButtonOpenKeyFile->setEnabled(false);
	ui->pushButtonOpenPartition->setEnabled(false);
	ui->pushButtonOpenVolume->setEnabled(false);
	ui->rbPassphrase->setEnabled(false);
	ui->rbPassphraseFromFile->setEnabled(false);
}

void luksdeletekey::enableAll()
{
	ui->groupBox->setEnabled(true);
	ui->label->setEnabled(true);
	ui->labelPassphrase->setEnabled(true);
	ui->lineEditPassphrase->setEnabled(true);
	ui->lineEditVolumePath->setEnabled(true);
	ui->pushButtonCancel->setEnabled(true);
	ui->pushButtonDelete->setEnabled(true);
	ui->pushButtonOpenKeyFile->setEnabled(true);
	ui->pushButtonOpenPartition->setEnabled(true);
	ui->pushButtonOpenVolume->setEnabled(true);
	ui->rbPassphrase->setEnabled(true);
	ui->rbPassphraseFromFile->setEnabled(true);
}

void luksdeletekey::pbCancel()
{
	HideUI() ;
}

void luksdeletekey::pbOpenPartition()
{
	//HideUI();
	emit pbOpenPartitionClicked();
}
void luksdeletekey::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.exec() ;
}
void luksdeletekey::pbDelete()
{
	QMessageBox m ;
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	if ( ui->lineEditPassphrase->text().isEmpty() == true ){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("ERROR: the passphrase field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}
	if ( ui->lineEditVolumePath->text().isEmpty() == true ){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("ERROR: the path to encrypted volume field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}

	if ( zuluCrypt::isLuks(ui->lineEditVolumePath->text()) == false ){

		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("given path does not point to a luks volume"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}

	QProcess N ;
	N.start(QString(ZULUCRYPTzuluCrypt) + QString(" emptyslots ") + ui->lineEditVolumePath->text());
	N.waitForFinished() ;

	QByteArray b = N.readAllStandardOutput() ;

	if(b.at(0) == '3'){
		m.setWindowTitle(QString("WARNING!"));
		m.setText(QString("There is only one last key in the volume.\nDeleting it will make the volume unopenable and lost forever.\nAre you sure you want to delete this key?"));

		m.addButton(QMessageBox::Yes);
		m.addButton(QMessageBox::No);

		if( m.exec() == QMessageBox::No )
			return ;
	}

	disableAll();

	ldk = new luksdeleteKeyThread(ui,&status) ;

	connect(ldk,SIGNAL(finished()),this,SLOT(threadfinished())) ;

	ldk->start();
}

void luksdeletekey::threadfinished()
{
	delete ldk ;

	switch( status ){
		case 0 :
			UIMessage(QString("SUCCESS"),QString("key successfully removed\n") + zuluCrypt::luksEmptySlots(ui->lineEditVolumePath->text()) + QString(" / 8 slots are now in use"));
			HideUI() ;
			return ;
			break ;
		case 1 :UIMessage(QString("ERROR"),QString("one or more paths has an empty space in them, the back end doesnt like it"));
			break ;
		case 2 :UIMessage(QString("ERROR"),QString("there is no key in the volume that match entered key"));
			break ;
		case 4 :UIMessage(QString("ERROR"),QString("device does not exist"));
			break ;
		case 5 :UIMessage(QString("ERROR"),QString("key file does not exist"));
			break ;

		default:UIMessage(QString("ERROR"),QString( "un unexpected error has occured, key not removed "));
	}
	enableAll();
}

void luksdeletekey::pbOpenVolume()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("volume path"),QDir::homePath(),0);
	ui->lineEditVolumePath->setText( Z );
}

void luksdeletekey::deleteKey(QString passphrase)
{
	enableAll();
	ui->lineEditVolumePath->setText(passphrase);
	ShowUI() ;
}

void luksdeletekey::HideUI()
{
	ui->lineEditPassphrase->clear();
	ui->lineEditVolumePath->clear();
	this->hide();
}

luksdeletekey::~luksdeletekey()
{
	delete pUI ;

	delete ui ;
}
