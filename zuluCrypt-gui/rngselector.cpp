#include "rngselector.h"
#include "ui_rngselector.h"

#include <QFile>
#include <QDir>

rngselector::rngselector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rngselector)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());

	connect(ui->pbOk,SIGNAL(clicked()),this,SLOT(pbOk()));

	connect(ui->rbRandom,SIGNAL(clicked()),this,SLOT(rbRandom()));

	connect(ui->rbUrandom,SIGNAL(clicked()),this,SLOT(rbUrandom())) ;
}

void rngselector::pbOk()
{
	this->hide();
}

void rngselector::rbRandom()
{
	QFile f(QDir::homePath() + QString("/.zuluCrypt/rng")) ;

	f.open(QIODevice::WriteOnly | QIODevice::Truncate) ;

	f.write("/dev/random") ;

	f.close();
}

void rngselector::rbUrandom()
{
	QFile f(QDir::homePath() + QString("/.zuluCrypt/rng")) ;

	f.open(QIODevice::WriteOnly | QIODevice::Truncate) ;

	f.write("/dev/urandom") ;

	f.close();
}

void rngselector::ShowUI()
{
	QFile f(QDir::homePath() + QString("/.zuluCrypt/rng")) ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray b = f.readAll() ;

	if( b.contains("/dev/random") == true )
		ui->rbRandom->setChecked(true);
	else
		ui->rbUrandom->setChecked(true);

	f.close();

	this->show();
}

rngselector::~rngselector()
{
	delete ui;
}
