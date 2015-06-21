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

#include "mountoptions.h"
#include "ui_mountoptions.h"

#include <QString>
#include <QCloseEvent>

mountOptions::mountOptions( QString * options,QWidget * parent ) :
	QDialog( parent ),m_ui( new Ui::mountOptions ),m_options( options )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;
	connect( m_ui->pbOk,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
}

void mountOptions::ShowUI()
{
	m_ui->lineEditOptions->setFocus() ;
	this->show() ;
}

mountOptions::~mountOptions()
{
	delete m_ui ;
}

void mountOptions::pbOK()
{
	if( m_options ){
		*m_options = m_ui->lineEditOptions->text() ;
	}
	this->HideUI() ;
}

void mountOptions::pbCancel()
{
	if( m_options ){
		*m_options = QString() ;
	}
	this->HideUI() ;
}

void mountOptions::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void mountOptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
