/*
 *
 *  Copyright (c) 2014-2015
 *  name : Francis Banyikwa
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


#include "deviceoffset.h"
#include "ui_deviceoffset.h"
#include "../zuluCrypt-gui/utility.h"

deviceOffset::deviceOffset( QWidget * parent ) :QDialog( parent ),m_ui( new Ui::deviceOffset )
{
	m_ui->setupUi( this ) ;
	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	this->installEventFilter( this ) ;
}

bool deviceOffset::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->closeUI() ; } ) ;
}

void deviceOffset::ShowUI()
{
	this->show() ;
}

void deviceOffset::ShowUI_1()
{
	m_ui->label_2->setEnabled( false ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	this->show() ;
}

deviceOffset::~deviceOffset()
{
	delete m_ui ;
}

void deviceOffset::closeUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void deviceOffset::pbOK()
{
	emit offSetValue( m_ui->lineEdit->text(),m_ui->lineEditKey->text() ) ;
	this->closeUI() ;
}

void deviceOffset::pbCancel()
{
	this->closeUI() ;
}

void deviceOffset::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->closeUI() ;
}
