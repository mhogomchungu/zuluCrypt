#include "createkeyfile.h"
#include "ui_createkeyfile.h"
#include "executables.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <iostream>

createkeyfile::createkeyfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createkeyfile)
{
	ui->setupUi(this);

	connect(ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
	connect(ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;
	connect(ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;

}

void createkeyfile::HideUI()
{
	this->hide();
}

void createkeyfile::ShowUI()
{
	ui->lineEditFileName->clear();
	ui->lineEditPath->clear();
	this->show();
}

void createkeyfile::pbCancel()
{
	this->HideUI();
}

void createkeyfile::pbCreate()
{
	QMessageBox m ;
	m.addButton(QMessageBox::Ok);
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	if( ui->lineEditFileName->text().isEmpty() == true ){

		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("the key name field is empth"));
		m.exec() ;
		return ;
	}
	if( ui->lineEditPath->text().isEmpty() == true ){

		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("folder path to where the key will be created is empty"));
		m.exec() ;
		return ;
	}

	QString dd = QString(ZULUCRYPTdd) + QString(" if=/dev/urandom of=") + ui->lineEditPath->text() + QString("/") + ui->lineEditFileName->text() + QString(" bs=512 count=1") ;

	std::cout << dd.toStdString() << std::endl ;

	QProcess p ;
	p.start( dd ) ;
	p.waitForFinished() ;

	if( p.exitCode() == 0 ){

		m.setWindowTitle(QString("SUCCESS"));
		m.setText(QString("key file successfully created"));
		m.exec();
		this->HideUI();

	}else{
		m.setWindowTitle(QString("ERROR"));
		m.setText(QString("could not create the key file.\nDo you have writing access to destination folder? "));
		m.exec() ;
	}
}

void createkeyfile::pbOpenFolder()
{
	QString Z = QFileDialog::getExistingDirectory((QWidget *) this,QString("Select a folder to create a key file in"),QDir::homePath(),QFileDialog::ShowDirsOnly) ;

	ui->lineEditPath->setText( Z );
}

createkeyfile::~createkeyfile()
{
	delete ui;
}
