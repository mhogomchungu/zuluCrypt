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

#include "createpartition.h"
#include "ui_createpartition.h"
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <QAbstractButton>
#include <QProcess>
#include <QThread>

#include "executables.h"

createpartition::createpartition(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createpartition)
{
	ui->setupUi(this);

	this->setFixedSize(this->size());

	ui->lineEditVolumePath->setEnabled(false);

	ui->lineEditPassphrase1->setFocus();

	connect(ui->pbOpenKeyFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenKeyFile()));
	connect(ui->pbCreate,
		SIGNAL(clicked()),
		this,
		SLOT(pbCreateClicked()));
	connect(ui->pbCancel,
		SIGNAL(clicked()),
		this,
		SLOT(pbCancelClicked()));
	connect(ui->rbPassphrase,
		SIGNAL(clicked()),
		this,
		SLOT(rbPassphraseClicked()));
	connect(ui->rbPassphraseFromFile,
		SIGNAL(clicked()),
		this,
		SLOT(rbPasssphraseFromFileClicked()));

	connect(ui->comboBoxVolumeType,
		SIGNAL(currentIndexChanged(int)),
		this,
		SLOT(rng(int))) ;
}

void createpartition::rng(int s)
{
	if( s == 1)
		ui->comboBoxRNG->setEnabled(false);
	else
		ui->comboBoxRNG->setEnabled(true);
}

void createpartition::closeEvent(QCloseEvent *e)
{
	e->ignore();
	pbCancelClicked() ;
}

void createpartition::ShowPartitionUI(QString volume)
{
	ShowUI(tr("path to partition"),volume);
}

void createpartition::ShowFileUI(QString volume)
{
	ShowUI(tr("path to file"),volume);
}

void::createpartition::ShowUI(QString l,QString v)
{
	enableAll();
	ui->labelVolumePath->setText(l);
	ui->lineEditVolumePath->setText(v);
	ui->rbPassphrase->setChecked(true);
	ui->pbOpenKeyFile->setEnabled(false);
	ui->lineEditVolumePath->setEnabled(false);
	this->rbPassphraseClicked() ;
	ui->lineEditPassphrase1->setFocus();
	ui->comboBoxFS->setCurrentIndex(0);
	ui->comboBoxRNG->setCurrentIndex(0);
	ui->comboBoxVolumeType->setCurrentIndex(0);
	ui->labelRepeatPassPhrase->setEnabled(true);
	created = false ;
	this->show();
}

void createpartition::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("key file path"),
						 QDir::homePath(),
						 0);
	ui->lineEditPassphrase1->setText(Z);
}

void createpartition::pbCancelClicked()
{
	/*
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(tr("Are you sure you want to cancel this operation?"));
	m.setWindowTitle(tr("WARNING"));
	m.addButton(QMessageBox::Yes);
	m.addButton(QMessageBox::No);
	m.setFont(this->font());

	if( m.exec() == QMessageBox::No )
		return ;
*/
	if( created == false ){
		QString s = ui->lineEditVolumePath->text() ;
		if(s.left(5) != QString("/dev/"))
			QFile::remove(s) ;
	}

	HideUI() ;
}

void createpartition::HideUI()
{
	this->hide();
}

void createpartition::enableAll()
{
	ui->labelPassPhrase->setEnabled(true);
	ui->labelVolumePath->setEnabled(true);
	ui->labelRepeatPassPhrase->setEnabled(true);
	ui->lineEditPassphrase1->setEnabled(true);
	ui->lineEditPassPhrase2->setEnabled(true);
	ui->lineEditVolumePath->setEnabled(true);
	ui->pbCancel->setEnabled(true);
	ui->pbCreate->setEnabled(true);
	ui->pbOpenKeyFile->setEnabled(true);
	ui->labelfs->setEnabled(true);
	ui->labelvolumetype->setEnabled(true);
	ui->labelrng->setEnabled(true);
	ui->comboBoxFS->setEnabled(true);
	ui->comboBoxVolumeType->setEnabled(true);
	if(ui->comboBoxVolumeType->currentIndex() == 0)
		ui->comboBoxRNG->setEnabled(true);
	ui->rbPassphrase->setEnabled(true);
	ui->rbPassphraseFromFile->setEnabled(true);
}

void createpartition::disableAll()
{
	ui->labelPassPhrase->setEnabled(false);
	ui->labelVolumePath->setEnabled(false);
	ui->labelRepeatPassPhrase->setEnabled(false);
	ui->lineEditPassphrase1->setEnabled(false);
	ui->lineEditPassPhrase2->setEnabled(false);
	ui->lineEditVolumePath->setEnabled(false);
	ui->pbCancel->setEnabled(false);
	ui->pbCreate->setEnabled(false);
	ui->pbOpenKeyFile->setEnabled(false);
	ui->labelfs->setEnabled(false);
	ui->labelrng->setEnabled(false);
	ui->labelvolumetype->setEnabled(false);
	ui->comboBoxFS->setEnabled(false);
	ui->comboBoxVolumeType->setEnabled(false);
	ui->comboBoxRNG->setEnabled(false);
	ui->rbPassphrase->setEnabled(false);
	ui->rbPassphraseFromFile->setEnabled(false);
}

void createpartition::rbPassphraseClicked()
{
	ui->pbOpenKeyFile->setEnabled(false);
	ui->lineEditPassPhrase2->setEnabled(true);
	ui->lineEditPassphrase1->clear();
	ui->lineEditPassPhrase2->clear();
	ui->lineEditPassphrase1->setEchoMode(QLineEdit::Password);
	ui->lineEditPassPhrase2->setEchoMode(QLineEdit::Password);
	ui->labelPassPhrase->setText(tr("passphrase"));
	ui->labelRepeatPassPhrase->setEnabled(true);
}

void createpartition::rbPasssphraseFromFileClicked()
{
	ui->pbOpenKeyFile->setEnabled(true);
	ui->lineEditPassphrase1->clear();
	ui->lineEditPassPhrase2->clear();
	ui->lineEditPassphrase1->setEchoMode(QLineEdit::Normal);
	ui->lineEditPassPhrase2->setEnabled(false);
	ui->labelPassPhrase->setText(tr("key file"));
	ui->labelRepeatPassPhrase->setEnabled(false);
}

void createpartition::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.setFont(this->font());
	m.exec() ;
}

void createpartition::pbCreateClicked()
{
	if( ui->lineEditVolumePath->text().isEmpty() == true )	{

		UIMessage(tr("ERROR!"),tr("volume path field is empty"));
		return ;
	}

	if( ui ->lineEditPassphrase1->text().isEmpty() == true){

		if( ui->rbPassphrase->isChecked() == true)
			UIMessage(tr("ERROR"),tr("passphrases field is empty"));
		else
			UIMessage(tr("ERROR"),tr("key file field is empty"));

		return ;
	}

	if(ui->rbPassphrase->isChecked() == true){

		if( QString::compare(ui->lineEditPassphrase1->text(),
				     ui->lineEditPassPhrase2->text()) != 0 ){
			UIMessage(tr("ERROR"),tr("passphrases do not match"));
			return ;
		}
	}

	QMessageBox m ;
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setWindowTitle(tr("WARNING"));

	m.addButton(QMessageBox::Yes);
	m.addButton(QMessageBox::No);
	m.setDefaultButton(QMessageBox::No);

	QString wr = tr("all contents of \"") ;
	wr = wr + ui->lineEditVolumePath->text() + tr("\" will be deleted!.");
	wr = wr + tr("\nAre you sure you want to proceed?") ;
	m.setText(wr);

	if ( m.exec() != QMessageBox::Yes )
		return ;

	QString source ;

	QString passphrase = ui->lineEditPassphrase1->text() ;

	if (ui->rbPassphraseFromFile->isChecked() == true)
		source = QString("-f") ;
	else{
		source = QString("-p") ;

		passphrase.replace("\"","\"\"\"") ;
	}

	QString exe = QString(ZULUCRYPTzuluCrypt) ;
	exe = exe + QString(" create \"") ;
	exe = exe + ui->lineEditVolumePath->text() + QString("\" ") ;
	exe = exe + ui->comboBoxFS->currentText() + QString(" ") ;
	exe = exe + ui->comboBoxVolumeType->currentText() + QString(" ") ;
	exe = exe +  source + QString(" \"") ;;
	exe = exe + passphrase + QString("\" ") ;;
	exe = exe + ui->comboBoxRNG->currentText();

	cvt = new runInThread(exe,&status) ;

	connect(cvt,SIGNAL(finished()),this,SLOT(threadfinished())) ;

	disableAll();

	cvt->start();
}

void createpartition::threadfinished()
{	
	created = true ;

	delete cvt ;

	switch( status ) {
		case 0 : UIMessage(tr("SUCCESS"),
				   tr("volume created successfully"));
			HideUI();
			break;
		case 1 : UIMessage(tr("ERROR"),
			tr("File path given does not point to a file or partition"));
			break ;			
		case 6 : UIMessage(tr("ERROR"),
				   tr("couldnt get requested memory to open the key file"));
			break ;
		default: UIMessage(tr("ERROR"),
				   (tr("unrecognized error has occured,volume not created")));
	}
	enableAll();
}

createpartition::~createpartition()
{
    delete ui;
}
