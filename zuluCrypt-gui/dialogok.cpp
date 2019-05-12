/*
 *
 *  Copyright ( c ) 2016
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

#include "dialogok.h"
#include "ui_dialogok.h"
#include "utility.h"
#include <QMessageBox>

static QWidget * _setParent( QWidget * s )
{
	auto m = utility::mainWindowWidget() ;

	if( m != s ){

		return m ;
	}else{
		return s ;
	}
}

dialogok::dialogok( QWidget * parent,bool s,bool q,const QString& e,const QString& f ) :
	QDialog( _setParent( parent ) ),m_ui( new Ui::dialogok ),
	m_parent( parent ),m_mainWindow( utility::mainWindowWidget() )
{
	if( m_parent != m_mainWindow ){

		m_parent->hide() ;
	}

	m_ui->setupUi( this ) ;
	m_ui->label->setText( f ) ;

	this->setWindowTitle( e ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked( bool ) ),this,SLOT( ok() ) ) ;
	connect( m_ui->pbNO,SIGNAL( clicked( bool ) ),this,SLOT( no() ) ) ;
	connect( m_ui->pbYES,SIGNAL( clicked( bool ) ),this,SLOT( yes() ) ) ;

	m_ui->pbNO->setVisible( s ) ;
	m_ui->pbYES->setVisible( s ) ;
	m_ui->pbOK->setVisible( !s ) ;

	if( s ){

		if( q ){

			m_ui->pbNO->setFocus() ;
		}else{
			m_ui->pbYES->setFocus() ;
		}
	}else{
		m_ui->pbOK->setFocus() ;
	}
}

void dialogok::ok()
{
	m_buttonRole = QMessageBox::Ok ;
	this->done( 0 ) ;
}

void dialogok::yes()
{
	m_buttonRole = QMessageBox::Yes ;
	this->done( 0 ) ;
}

void dialogok::no()
{
	m_buttonRole = QMessageBox::No ;
	this->done( 0 ) ;
}

void dialogok::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
}

dialogok::~dialogok()
{
	delete m_ui ;
}

QMessageBox::StandardButton dialogok::Show()
{
	this->show() ;
	this->exec() ;

	if( m_parent != m_mainWindow ){

		m_parent->show() ;
	}

	return m_buttonRole ;
}
