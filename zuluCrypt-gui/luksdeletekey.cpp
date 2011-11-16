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
#include "../zuluCrypt-cli/executables.h"

#include <QObject>
#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QIcon>

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

	ldk = NULL ;

	connect(pUI,
		SIGNAL(clickedPartition(QString)),
		this,
		SLOT(deleteKey(QString)));

	connect(this,
		SIGNAL(pbOpenPartitionClicked()),
		pUI,
		SLOT(ShowUI()));

	connect(ui->pushButtonDelete,
		SIGNAL(clicked()),
		this,
		SLOT(pbDelete())) ;

	connect(ui->pushButtonCancel,
		SIGNAL(clicked()),
		this,
		SLOT(pbCancel())) ;

	connect(ui->rbPassphrase,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbPassphrase())) ;

	connect(ui->rbPassphraseFromFile,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbPassphraseFromFile())) ;

	connect(ui->pushButtonOpenKeyFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenKeyFile())) ;
	connect(ui->pushButtonOpenVolume,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenVolume()));

	connect(ui->pushButtonOpenPartition,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenPartition())) ;
}

void luksdeletekey::closeEvent(QCloseEvent *e)
{
	e->ignore();

	if( ldk == NULL )
		pbCancel();
}

void luksdeletekey::rbPassphrase()
{
	ui->labelPassphrase->setText(tr("passphrase"));
	ui->lineEditPassphrase->setEchoMode(QLineEdit::Password);
	ui->lineEditPassphrase->clear();
	ui->pushButtonOpenKeyFile->setEnabled(false);
	ui->pushButtonOpenKeyFile->setIcon(QIcon(QString(":/passphrase.png")));

}

void luksdeletekey::rbPassphraseFromFile()
{
	ui->labelPassphrase->setText(tr("keyfile"));
	ui->lineEditPassphrase->setEchoMode(QLineEdit::Normal);
	ui->lineEditPassphrase->clear();
	ui->pushButtonOpenKeyFile->setEnabled(true);
	ui->pushButtonOpenKeyFile->setIcon(QIcon(QString(":/keyfile.png")));
}

void luksdeletekey::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("key file with a passphrase to delete"),
						 QDir::homePath(),
						 0);
	ui->lineEditPassphrase->setText( Z );
}

void luksdeletekey::ShowUI()
{
	enableAll();
	ui->rbPassphrase->setEnabled(true);
	ui->labelPassphrase->setText(tr("passphrase"));
	ui->rbPassphrase->setChecked(true);
	ui->lineEditVolumePath->setFocus();
	ui->pushButtonCancel->setDefault(true);
	ui->pushButtonOpenPartition->setIcon(QIcon(QString(":/partition.png")));
	ui->pushButtonOpenVolume->setIcon(QIcon(QString(":/file.png")));
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

	volumePath = ui->lineEditVolumePath->text() ;

	QString passphrase = ui->lineEditPassphrase->text() ;

	if ( passphrase.isEmpty() == true ){
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("the passphrase field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}

	if( volumePath.mid(0,2) == QString("~/"))
		volumePath = QDir::homePath() + QString("/") + volumePath.mid(2) ;

	if ( volumePath.isEmpty() == true ){
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("the path to encrypted volume field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}

	if(QFile::exists(volumePath) == false){
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("volume path field does not point to a file or device"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}
	if ( zuluCrypt::isLuks(volumePath) == false ){

		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("given path does not point to a luks volume"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}

	if(ui->rbPassphraseFromFile->isChecked() == true){

		if( passphrase.mid(0,2) == QString("~/"))
			passphrase = QDir::homePath() + QString("/") + passphrase.mid(2);

		if(QFile::exists(passphrase) == false){
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("key file field does not point to a file"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			return ;
		}
	}

	if(zuluCrypt::luksEmptySlots(volumePath).at(0) == QString("1")){
		QString s = tr("There is only one last key in the volume.");
		s = s + tr("\nDeleting it will make the volume unopenable and lost forever.") ;
		s = s + tr("\nAre you sure you want to delete this key?");

		m.setWindowTitle(tr("WARNING!"));
		m.setText(s) ;

		m.addButton(QMessageBox::Yes);
		m.addButton(QMessageBox::No);
		m.setDefaultButton(QMessageBox::No);

		if( m.exec() == QMessageBox::No )
			return ;
	}

	QString exe = QString(ZULUCRYPTzuluCrypt) ;
	exe = exe + QString(" removekey ")  ;
	exe = exe + QString("\"") +  volumePath + QString("\"") ;

	if ( ui->rbPassphraseFromFile->isChecked() == true ){

		exe = exe + QString(" -f ") ;

	}else{
		exe = exe + QString(" -p ") ;

		passphrase = passphrase.replace("\"","\"\"\"") ;
	}

	exe = exe + QString(" \"") + passphrase + QString("\"") ;

	disableAll();

	ldk = new runInThread(exe,&status) ;

	connect(ldk,
		SIGNAL(finished()),
		this,
		SLOT(threadfinished())) ;

	ldk->start();
}

void luksdeletekey::threadfinished()
{
	delete ldk ;

	ldk = NULL ;

	QStringList l = zuluCrypt::luksEmptySlots(volumePath) ;

	switch( status ){
		case 0 :
			UIMessage(tr("SUCCESS"),
				  tr("key removed successfully.\n") + \
				  l.at(0) + QString(" / ") + l.at(1) + \
				  tr(" slots are now in use"));
			HideUI() ;
			return ;
			break ;
		case 1 :UIMessage(tr("ERROR"),tr("device is not a luks device or does not exist"));
			break ;
		case 2 :UIMessage(tr("ERROR"),tr("there is no key in the volume that match entered key"));
			break ;
		case 3 :UIMessage(tr("ERROR"),tr("could not open luks device"));
			break ;
		case 7 :UIMessage(tr("ERROR"),tr("could not get enough memory to open the key file"));
			break ;

		default:UIMessage(tr("ERROR"),tr( "un unexpected error has occured, key not removed "));
	}
	enableAll();
}

void luksdeletekey::pbOpenVolume()
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("volume path"),
						 QDir::homePath(),
						 0);
	ui->lineEditVolumePath->setText( Z );
}

void luksdeletekey::deleteKey(QString path)
{
	enableAll();
	ui->lineEditVolumePath->setText(path);
	ui->lineEditPassphrase->setFocus();
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
