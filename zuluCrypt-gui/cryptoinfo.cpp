/*
 *  Copyright ( c ) 2012
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

static QString _optionPath()
{
	return QDir::homePath() + "/.zuluCrypt/doNotshowWarning.option" ;
}

cryptoinfo::cryptoinfo( QWidget * parent ) :
	QWidget( parent ),
	m_ui( new Ui::cryptoinfo )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	m_ui->checkBox->setChecked( utility::pathExists( _optionPath() ) ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->checkBox,SIGNAL( clicked( bool ) ),this,SLOT( checkBoxChecked( bool ) ) ) ;

	this->installEventFilter( this ) ;
}

bool cryptoinfo::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void cryptoinfo::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	HideUI() ;
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

bool cryptoinfo::Show()
{
	return utility::pathExists( _optionPath() ) == false ;
}

void cryptoinfo::checkBoxChecked( bool checked )
{
	QFile f( _optionPath() ) ;

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
