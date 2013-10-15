/*
 *
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
#include "createvolumedialog.h"
#include "ui_createvolumedialog.h"

CreateVolumeDialog::CreateVolumeDialog( const QString& path,QWidget *parent ) :
	QDialog( parent ),
	m_ui( new Ui::CreateVolumeDialog )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_path = path ;
	m_opt_count = 0 ;

	m_ui->pbNo->setFocus() ;

	connect( m_ui->pbYes,SIGNAL( clicked() ),this,SLOT( pbYes() ) ) ;
	connect( m_ui->pbNo,SIGNAL( clicked() ),this,SLOT( pbNo() ) ) ;
}

void CreateVolumeDialog::ShowUI()
{
	QString msg = tr( "This operation will lead to permanent destrunction \
of all present data in \"%1\".\n\nAre you sure you want to continue?" ).arg( m_path ) ;
	m_ui->label_1->setText( msg ) ;
	this->show() ;
}

void CreateVolumeDialog::pbNo()
{
	if( m_opt_count == 0 ){
		m_opt = 0 ;
	}else{
		m_opt = 1 ;
	}
	this->hide() ;
	this->deleteLater() ;
}

void CreateVolumeDialog::pbYes()
{
	m_ui->pbNo->setFocus() ;

	if( m_opt_count == 1 ){
		m_opt = 2 ;
		this->hide() ;
		this->deleteLater() ;
		return ;
	}
	m_opt_count = 1 ;
	QString msg = tr( "It is advised to create encrypted containers over random data to prevent information leakage.\n\n\
Do you want to write random data to \"%1\" first before creating an encrypted container in it?\n\n\
You can stop the random data writing process anytime you want if \
it takes too long and you can no longer wait.\n\n" ).arg( m_path ) ;
	m_ui->label_1->setText( msg ) ;
}

CreateVolumeDialog::~CreateVolumeDialog()
{
	emit dialogResult( m_opt ) ;
	delete m_ui ;
}
