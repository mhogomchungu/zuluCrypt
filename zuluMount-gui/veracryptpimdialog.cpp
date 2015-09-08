/*
 *
 *  Copyright (c) 2015
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

#include "veracryptpimdialog.h"
#include "ui_veracryptpimdialog.h"

#include "../zuluCrypt-gui/utility.h"
#include "../zuluCrypt-gui/dialogmsg.h"

VeraCryptPIMDialog::VeraCryptPIMDialog( QWidget * parent,std::function< void( int ) > function ) :
	QDialog( parent ),m_ui( new Ui::VeraCryptPIMDialog ),m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbSet,SIGNAL( clicked() ),this,SLOT( pbSet() ) ) ;

	m_ui->label->setText( tr( "Set VeraCrypt dynamic mode magic number below." ) ) ;

	m_ui->lineEditPIM->setFocus() ;

	this->Show() ;
}

bool VeraCryptPIMDialog::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->pbCancel() ; } ) ;
}

void VeraCryptPIMDialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void VeraCryptPIMDialog::Show()
{
	this->show() ;
}

void VeraCryptPIMDialog::Hide()
{
	this->hide() ;
	this->deleteLater() ;
}

VeraCryptPIMDialog::~VeraCryptPIMDialog()
{
	delete m_ui ;
}

void VeraCryptPIMDialog::pbSet()
{
	bool ok ;

	int e = m_ui->lineEditPIM->text().toInt( &ok ) ;

	if( ok ){

		m_function( e ) ;
		this->Hide() ;
	}else{
		DialogMsg msg( this ) ;

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to convert the value to digits only " ) ) ;
	}
}

void VeraCryptPIMDialog::pbCancel()
{
	m_function( 0 ) ;
	this->Hide() ;
}
