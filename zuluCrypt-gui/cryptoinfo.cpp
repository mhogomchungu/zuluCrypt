/*
 *  Copyright ( c ) 2012
 *  name : mhogo mchungu
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

cryptoinfo::cryptoinfo( QWidget * parent ) :
	QWidget( parent ),
	m_ui( new Ui::cryptoinfo )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;
	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
}

void cryptoinfo::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	HideUI() ;
}

void cryptoinfo::HideUI()
{
	emit closeUISignal() ;
	this->hide() ;
}

void cryptoinfo::pbOK()
{
	HideUI() ;
}

cryptoinfo::~cryptoinfo()
{
	delete m_ui ;
}
