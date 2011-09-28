#include "createpartition.h"
#include "ui_createpartition.h"
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <QAbstractButton>

createpartition::createpartition(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createpartition)
{
	ui->setupUi(this);

	ui->lineEditVolumePath->setEnabled(false);

	connect(ui->pbOpenKeyFile,SIGNAL(clicked()),this,SLOT(pbOpenKeyFile()));
	connect(ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreateClicked()));
	connect(ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancelClicked()));
	connect(ui->rbPassphrase,SIGNAL(clicked()),this,SLOT(rbPassphraseClicked()));
	connect(ui->rbPassphraseFromFile,SIGNAL(clicked()),this,SLOT(rbPasssphraseFromFileClicked()));
}

void createpartition::ShowPartitionUI(QString volume)
{
	ui->rbPassphrase->setChecked(true);
	ui->lineEditVolumePath->setText(volume);
	ui->rbLuks->setChecked(true);
	ui->rbext4->setChecked(true);
	ui->pbCancel->setFocus();
	ui->labelVolumePath->setText(QString("path to partition"));
	ui->labelPassPhrase->setText(QString("passphrase"));
	this->rbPassphraseClicked() ;
	this->show();
}

void createpartition::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("key file path"),QDir::homePath(),0);
	ui->lineEditPassphrase1->setText(Z);
}

void createpartition::pbCancelClicked()
{
	this->hide();
}

void createpartition::rbPassphraseClicked()
{
	ui->pbOpenKeyFile->setEnabled(false);
	ui->lineEditPassPhrase2->setEnabled(true);
	ui->lineEditPassphrase1->clear();
	ui->lineEditPassPhrase2->clear();
	ui->lineEditPassphrase1->setEchoMode(QLineEdit::Password);
	ui->lineEditPassPhrase2->setEchoMode(QLineEdit::Password);
	ui->labelPassPhrase->setText(QString("passphrase"));

}

void createpartition::rbPasssphraseFromFileClicked()
{
	ui->pbOpenKeyFile->setEnabled(true);
	ui->lineEditPassphrase1->clear();
	ui->lineEditPassPhrase2->clear();
	ui->lineEditPassphrase1->setEchoMode(QLineEdit::Normal);
	ui->lineEditPassPhrase2->setEnabled(false);
	ui->labelPassPhrase->setText(QString("key file"));
}

void createpartition::pbCreateClicked()
{
	QMessageBox m ;
	m.setWindowTitle(QString("ERROR!"));
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	if( ui->lineEditVolumePath->text().isEmpty() == true )	{
		m.addButton(QMessageBox::Ok);
		m.setText(QString("ERROR: volume path field is empty"));
		m.exec() ;
		return ;
	}

	QString fs ;

	if( ui->rbext3->isChecked() == true)
		fs = QString("ext3") ;
	else if( ui->rbext4->isChecked() == true)
		fs = QString("ext4") ;
	else
		fs = QString("fat") ;

	QString ct ;

	if(ui->rbPlain->isChecked() == true)
		ct = QString("plain");
	else
		ct = QString("luks");

	if( ui ->lineEditPassphrase1->text().isEmpty() == true){
		m.addButton(QMessageBox::Ok);
		if( ui->rbPassphrase->isChecked() == true)
			m.setText(QString("ERROR: passphrases field is empty"));
		else
			m.setText(QString("ERROR: key file field is empty"));

		m.exec();
		return ;
	}

	if(ui->rbPassphrase->isChecked() == true){

		if( QString::compare(ui->lineEditPassphrase1->text(),ui->lineEditPassPhrase2->text()) != 0 ){
			m.addButton(QMessageBox::Ok);
			m.setText(QString("ERROR: passphrases do not match"));
			m.exec();
			return ;
		}
	}

	QString r("This operation will delete all data in partition: " + ui->lineEditVolumePath->text()) ;
	r = r + QString("\nAre you sure you want to proceed?") ;

	m.addButton(QMessageBox::Yes);
	m.addButton(QMessageBox::No);
	m.setDefaultButton(QMessageBox::No);

	QString wr = QString("WARNING: all contents of ") + ui->lineEditVolumePath->text() + QString(" will be deleted");
	wr = wr + QString("\nAre you sure you want to proceed?") ;
	m.setText(wr);

	if ( m.exec() != QMessageBox::Yes )
		return ;

	this->hide();
	emit CreateVolume(fs,ct,ui->lineEditVolumePath->text(),ui->lineEditPassphrase1->text(),ui->rbPassphraseFromFile->isChecked()) ;
}

void createpartition::ShowFileUI(QString volume)
{
	ui->lineEditVolumePath->setText(volume);
	ui->rbLuks->setChecked(true);
	ui->rbext4->setChecked(true);
	ui->rbPassphrase->setChecked(true);
	ui->pbOpenKeyFile->setEnabled(false);
	ui->pbCancel->setFocus();
	ui->labelVolumePath->setText(QString("path to file"));
	this->show();
}

createpartition::~createpartition()
{
    delete ui;
}
