/*
 *
 *  Copyright ( c ) 2019
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

#include "createvolumeinexistingfile.h"
#include "ui_createvolumeinexistingfile.h"

#include "utility.h"

#include <QFileDialog>

createVolumeInExistingFIle::createVolumeInExistingFIle( QWidget * parent ) :
	QDialog( parent ),
	m_ui( new Ui::createVolumeInExistingFIle )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->pbSetFile->setIcon( QIcon( ":/file.png" ) ) ;

	connect( m_ui->pbSetFile,&QPushButton::clicked,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Enter Path To Existing File" ),utility::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditVolumePath->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbCancel,&QPushButton::clicked,[ this ](){

		this->Hide() ;
	} ) ;

	connect( m_ui->pbCreate,&QPushButton::clicked,[ this ](){

		this->create() ;
	} ) ;

	auto s = utility::plainDmCryptOptions() ;

	if( s.isEmpty() ){

		m_ui->cbVolumeProperties->addItem( "aes.cbc-essiv:256.256.ripemd160" ) ;
	}else{
		m_ui->cbVolumeProperties->addItems( s ) ;
	}

	this->show() ;
}

createVolumeInExistingFIle::~createVolumeInExistingFIle()
{
	delete m_ui ;
}

void createVolumeInExistingFIle::create()
{

}

void createVolumeInExistingFIle::Hide()
{
	this->hide() ;
	this->deleteLater() ;
}

void createVolumeInExistingFIle::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->Hide() ;
}
