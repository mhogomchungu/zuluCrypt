/*
 *  Copyright ( c ) 2012-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cryptoinfo.h"
#include "ui_cryptoinfo.h"

#include <QCloseEvent>
#include <QFile>

cryptoinfo::cryptoinfo( QWidget * parent,QString path,QString msg ) :
	QWidget( parent ),m_ui( new Ui::cryptoinfo ),m_path( path ),m_msg( msg )
{
	m_ui->setupUi( this ) ;

	if( !m_msg.isEmpty() ){

		m_ui->label->setText( m_msg ) ;
	}

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	m_ui->checkBox->setChecked( false ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->checkBox,SIGNAL( clicked( bool ) ),this,SLOT( checkBoxChecked( bool ) ) ) ;

	this->installEventFilter( this ) ;

	this->Show() ;
}

bool cryptoinfo::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void cryptoinfo::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void cryptoinfo::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void cryptoinfo::pbOK()
{
	this->HideUI() ;
}

void cryptoinfo::Show()
{
	if( utility::pathExists( m_path ) ){

		this->deleteLater() ;
	}else{
		this->show() ;
	}
}

void cryptoinfo::checkBoxChecked( bool checked )
{
	QFile f( m_path ) ;

	if( checked ){

		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	}else{
		f.remove() ;
	}
}

cryptoinfo::~cryptoinfo()
{
	delete m_ui ;
}
