/*
 *  Copyright (c) 2012
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
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

#include "dialogmsg.h"
#include "ui_dialogmsg.h"

DialogMsg::DialogMsg(QWidget *parent) :
        QDialog(parent),
	m_ui(new Ui::DialogMsg)
{
	m_ui->setupUi(this);
	this->setFont(parent->font());
	this->setModal(true);
	connect(m_ui->pbNo,SIGNAL(clicked()),this,SLOT(pbNo()));
	connect(m_ui->pbYes,SIGNAL(clicked()),this,SLOT(pbYes()));
	connect(m_ui->pbOk,SIGNAL(clicked()),this,SLOT(pbOK()));
}

void DialogMsg::pbNo()
{
	m_status = (int) QMessageBox::No ;
	this->hide();
}

void DialogMsg::pbOK()
{
	m_status = (int) QMessageBox::Ok ;
	this->hide();
}

void DialogMsg::pbYes()
{
	m_status = (int) QMessageBox::Yes ;
	this->hide();
}

void DialogMsg::closeEvent(QCloseEvent * e)
{
	e->ignore();
}

void DialogMsg::HideUI()
{
	m_status = -1 ;
	this->hide();
}

void DialogMsg::ShowUI(QString title, QString msg)
{
	m_ui->label->setText(msg);

	this->setWindowTitle(title);

	this->show();

	this->exec() ;
}

//#include "miscfunctions.h"
void DialogMsg::setDimentions(QString msg)
{
	int len = msg.size() ;

	//miscfunctions::debug(QString::number(msg.size())) ;

	//miscfunctions::debug(msg) ;
	if(len <= 58){
		this->setFixedSize(270,90);
		m_ui->label->setGeometry(10,10,251,31);
		m_ui->label->setFixedSize(m_ui->label->size());
		m_ui->pbOk->setGeometry(100,50,75,31);
		m_ui->pbYes->setGeometry(70,50,71,31);
		m_ui->pbNo->setGeometry(140,50,75,31);
	}else if(len <= 130){
		this->setFixedSize(372,118);
		m_ui->label->setGeometry(10,10,351,61);
		m_ui->label->setFixedSize(m_ui->label->size());
		m_ui->pbOk->setGeometry(150,80,75,31);
		m_ui->pbYes->setGeometry(120,80,71,31);
		m_ui->pbNo->setGeometry(190,80,75,31);
	}else if(len > 130){
		this->setFixedSize(372,146);
		m_ui->label->setGeometry(10,10,351,91);
		m_ui->label->setFixedSize(m_ui->label->size());
		m_ui->pbOk->setGeometry(150,110,75,31);
		m_ui->pbYes->setGeometry(120,110,71,31);
		m_ui->pbNo->setGeometry(190,110,75,31);
	}
}

void DialogMsg::ShowUIVolumeProperties(QString title, QString msg)
{
	m_ui->pbYes->setHidden(true);
	m_ui->pbNo->setHidden(true);
	m_ui->pbOk->setHidden(false);

	msg.replace("\t","\t\t") ;
	msg.replace("active slots:","slots:") ;

	m_ui->label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	this->setFixedSize(239,234);
	m_ui->label->setGeometry(10,10,221,191);
	m_ui->label->setFixedSize(m_ui->label->size());
	m_ui->pbOk->setGeometry(80,200,81,31);
	//m_ui->pbYes ->setGeometry(120,230,71,31);
	//m_ui->pbNo->setGeometry(190,230,75,31);

	this->ShowUI(title,msg);
}

void DialogMsg::ShowUIInfo(QString title,QString msg)
{
	m_ui->pbYes->setHidden(true);
	m_ui->pbNo->setHidden(true);
	m_ui->pbOk->setHidden(false);

	//m_ui->label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	this->setFixedSize(562,338);
	m_ui->label->setGeometry(10,10,541,271);
	m_ui->label->setFixedSize(m_ui->label->size());
	m_ui->pbOk->setGeometry(240,290,81,31);
	//m_ui->pbYes ->setGeometry(120,230,71,31);
	//m_ui->pbNo->setGeometry(190,230,75,31);

	this->ShowUI(title,msg);
}

void DialogMsg::ShowUIOK(QString title, QString msg)
{
	m_ui->pbYes->setHidden(true);
	m_ui->pbNo->setHidden(true);
	m_ui->pbOk->setHidden(false);

	this->setDimentions(msg);
	
	this->ShowUI(title,msg);
}

int DialogMsg::ShowUIYesNo(QString title, QString msg)
{
	m_ui->pbYes->setHidden(false);
	m_ui->pbNo->setHidden(false);
	m_ui->pbOk->setHidden(true);
	
	this->setDimentions(msg);

	this->ShowUI(title,msg);
	
	return m_status;
}

DialogMsg::~DialogMsg()
{
	delete m_ui;
}
