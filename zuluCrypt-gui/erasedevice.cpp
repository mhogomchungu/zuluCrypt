#include "erasedevice.h"
#include "ui_erasedevice.h"

erasedevice::erasedevice(QWidget *parent) :
        QWidget(parent),
	m_ui(new Ui::erasedevice)
{
	m_ui->setupUi(this);

	this->setFixedSize(this->size());
	this->setFont(parent->font());
	this->setWindowFlags(Qt::Window | Qt::Dialog);

	m_ui->progressBar->setValue(0);
	m_ui->progressBar->setMaximum(100);
	m_ui->progressBar->setMinimum(0);

	this->setWindowTitle(QString("write random data over existing data"));

	connect(m_ui->pushButtonFile,SIGNAL(clicked()),this,SLOT(pbFile()));
	connect(m_ui->pushButtonPartition,SIGNAL(clicked()),this,SLOT(pbPartition()));
	connect(m_ui->pushButtonStart,SIGNAL(clicked()),this,SLOT(pbStart()));
	connect(m_ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(pbCancel()));

	m_ui->pushButtonFile->setIcon(QIcon(QString(":/file.png")));
	m_ui->pushButtonPartition->setIcon(QIcon(QString(":/partition.png")));

	m_dt = NULL ;
	m_ui->lineEdit->setFocus();
}

void erasedevice::ShowUI()
{
	m_option = 0 ;
	UIMsg msg(this);

	QString msg_1 = tr("The next dialog will write random data to a device ");
	QString msg_2 = tr("leading to permanent loss of contents on the device.\n\n");
	QString msg_3 = tr("Are you sure you want to continue?");
	QString msg_4 = msg_1 + msg_2 + msg_3 ;

	if( msg.UIMessageWithConfirm(tr("WARNING"),msg_4) == QMessageBox::Yes )
		this->show();
	else
		this->HideUI();
}

void erasedevice::ShowUI(QString path)
{
	m_option = 1 ;
	m_ui->lineEdit->setText(path);
	this->show();
	this->pbStart();
}

void erasedevice::threadExitStatus(int st)
{
	this->setWindowTitle(QString("write random data over existing data"));

	m_dt = NULL ;

	UIMsg msg(this);

	switch(st){
		case 0 : m_ui->progressBar->setValue(100);
			 msg.UIMessage(tr("SUCCESS!"),tr("data on the device successfully erased")) ;
			 this->HideUI();
			 break ;
		case 1 : msg.UIMessage(tr("INFO!"),tr("operation terminated per user choice")) ;
			 break ;
		case 2 : msg.UIMessage(tr("ERROR!"),tr("could not write random data to device")) ;
			 break ;
	}

	if( m_option == 1 )
		this->HideUI();
	this->enableAll();
}

void erasedevice::HideUI()
{
	emit HideUISignal();
	this->hide();
}

void erasedevice::pbStart()
{
	this->setWindowTitle(QString("writing random data over existing data"));

	QString path = miscfunctions::resolveHomeSymbol(m_ui->lineEdit->text()) ;

	UIMsg msg(this);
	
	if(path.isEmpty())
		return msg.UIMessage(tr("ERROR!"),tr("device path field is empty"));
	
	if(miscfunctions::exists(path) == false)
		return msg.UIMessage(tr("ERROR!"),tr("invalid path to device"));
	
	if( m_option == 0 ){
		QString x_1 = tr("Are you really sure you want to write random data to \"%1\"").arg(path)	;
		QString x_2 = tr(" effectively destroying all contents in it?");
		QString x_3 = x_1 + x_2 ;

		if( msg.UIMessageWithConfirm(tr("WARNING!"),x_3) == QMessageBox::No )
			return ;
	}

	this->disableAll();

	path.replace("\"","\"\"\"");
	
	m_cancelClicked = false ;

	m_dt = new erasedevicethread(path) ;
	connect(m_dt,SIGNAL(progress(int)),this,SLOT(setProgress(int)));
	connect(m_dt,SIGNAL(finished()),m_dt,SLOT(deleteLater()));
	connect(m_dt,SIGNAL(exitStatus(int)),this,SLOT(threadExitStatus(int)));
	m_dt->start();
}

void erasedevice::enableAll()
{
	m_ui->label->setEnabled(true);
	m_ui->labelPath->setEnabled(true);
	m_ui->lineEdit->setEnabled(true);
	m_ui->pushButtonCancel->setEnabled(true);
	m_ui->pushButtonFile->setEnabled(true);
	m_ui->pushButtonPartition->setEnabled(true);
	m_ui->pushButtonStart->setEnabled(true);
}

void erasedevice::disableAll()
{
	m_ui->label->setEnabled(false);
	m_ui->labelPath->setEnabled(false);
	m_ui->lineEdit->setEnabled(false);
	//m_ui->pushButtonCancel->setEnabled(false); //need this one enabled
	m_ui->pushButtonFile->setEnabled(false);
	m_ui->pushButtonPartition->setEnabled(false);
	m_ui->pushButtonStart->setEnabled(false);
}

void erasedevice::setProgress(int st)
{
	m_ui->progressBar->setValue(st);
}

void erasedevice::pbCancel()
{
	/*
	 * m_cancelClicked is added here to prevents a crash when a user cancel
	 * the operation multiple times calling "m_dt->cancel()" multiple times.
	 */
	if(m_cancelClicked)
		return ;

	m_cancelClicked = true ;

	if(m_dt == NULL)
		this->HideUI();
	else
		m_dt->cancel();
}

void erasedevice::pbFile()
{
	QString Z = QFileDialog::getOpenFileName(this,tr("enter path to volume to be erased"),QDir::homePath(),0);
	m_ui->lineEdit->setText(Z);
}

void erasedevice::pbPartition()
{
	openpartition * openPartition = new openpartition(this);
	connect(openPartition,SIGNAL(clickedPartition(QString)),this,SLOT(setPath(QString)));
	connect(openPartition,SIGNAL(HideUISignal()),openPartition,SLOT(deleteLater()));
	openPartition->partitionList(tr("select a non system partition to erase its contents"),QString(" -N"));
}

void erasedevice::setPath(QString p)
{
	m_ui->lineEdit->setText(p);
}

void erasedevice::closeEvent(QCloseEvent * e)
{
	e->ignore();
	this->pbCancel();
}

erasedevice::~erasedevice()
{
	delete m_ui;
}
