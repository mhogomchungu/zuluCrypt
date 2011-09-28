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

#include <QObject>
#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>

luksdeletekey::luksdeletekey(QWidget *parent) :  QDialog(parent)
{
	ui.setupUi(this);
	this->setFixedSize(this->size());

	pUI.setParent(this);
	pUI.setWindowFlags(Qt::Window | Qt::Dialog);

	connect((QObject *)&pUI,SIGNAL(clickedPartition(QString)),this,SLOT(deleteKey(QString)));
	connect(this,SIGNAL(pbOpenPartitionClicked()),(QObject *)&pUI,SLOT(ShowUI()));
	connect(ui.pushButtonDelete,SIGNAL(clicked()),this,SLOT(pbDelete())) ;
	connect(ui.pushButtonCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(ui.rbPassphrase,SIGNAL(toggled(bool)),this,SLOT(rbPassphrase())) ;
	connect(ui.rbPassphraseFromFile,SIGNAL(toggled(bool)),this, SLOT(rbPassphraseFromFile())) ;
	connect(ui.pushButtonOpenKeyFile,SIGNAL(clicked()),this,SLOT(pbOpenKeyFile())) ;
	connect(ui.pushButtonOpenVolume,SIGNAL(clicked()),this,SLOT(pbOpenVolume()));
	connect(ui.pushButtonOpenPartition,SIGNAL(clicked()),this,SLOT(pbOpenPartition())) ;
}

void luksdeletekey::rbPassphrase()
{
	ui.labelPassphrase->setText(QString("passphrase"));
	ui.lineEditPassphrase->setEchoMode(QLineEdit::Password);
	ui.lineEditPassphrase->clear();
	ui.pushButtonOpenKeyFile->setEnabled(false);
}

void luksdeletekey::rbPassphraseFromFile()
{
	ui.labelPassphrase->setText(QString("keyfile"));
	ui.lineEditPassphrase->setEchoMode(QLineEdit::Normal);
	ui.lineEditPassphrase->clear();
	ui.pushButtonOpenKeyFile->setEnabled(true);
}

void luksdeletekey::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("key file with a passphrase to delete"),QDir::homePath(),0);
	ui.lineEditPassphrase->setText( Z );
}

void luksdeletekey::ShowUI()
{
	ui.rbPassphrase->setEnabled(true);
	ui.labelPassphrase->setText(QString("passphrase"));
	ui.rbPassphrase->setChecked(true);
	ui.lineEditVolumePath->setFocus();
	ui.pushButtonCancel->setDefault(true);
	this->show();
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

void luksdeletekey::pbDelete()
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	if ( ui.lineEditPassphrase->text().isEmpty() == true ){
		m.setText(QString("ERROR: the passphrase field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}
	if ( ui.lineEditVolumePath->text().isEmpty() == true ){
		m.setText(QString("ERROR: the path to encrypted volume field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}
	QString Z = ui.lineEditPassphrase->text() ;
	QString N = ui.lineEditVolumePath->text() ;
	bool P = ui.rbPassphraseFromFile->isChecked() ;

	//add quotation marks to prevent zuluCrypt-cli from getting confused
	//Z = "\"" + Z + "\"" ;
	//N = "\"" + N + "\"" ;

	HideUI() ;
	emit pbDeleteClicked( N, P, Z );
}

void luksdeletekey::pbOpenVolume()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("volume path"),QDir::homePath(),0);
	ui.lineEditVolumePath->setText( Z );
}

void luksdeletekey::deleteKey(QString passphrase)
{
	ui.lineEditVolumePath->setText(passphrase);
	ShowUI() ;
}

void luksdeletekey::HideUI()
{
	ui.lineEditPassphrase->clear();
	ui.lineEditVolumePath->clear();
	this->hide();
}

luksdeletekey::~luksdeletekey()
{
}
