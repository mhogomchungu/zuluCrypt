/*
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

#include "warnwhenextendingcontainerfile.h"
#include "ui_warnwhenextendingcontainerfile.h"

#include "utility.h"

void warnWhenExtendingContainerFile::Show( QWidget * parent,std::function< void() > function )
{
	if( utility::showWarningOnExtendingCoverFile() ){

		new warnWhenExtendingContainerFile( parent,function ) ;
	}else{
		function() ;
	}
}

warnWhenExtendingContainerFile::warnWhenExtendingContainerFile( QWidget * parent,std::function< void() > function ) :
	QDialog( parent ),
	m_ui( new Ui::warnWhenExtendingContainerFile ),
	m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	connect( m_ui->pbOk,&QPushButton::pressed,[ this ](){

		this->Hide() ;
	} ) ;

	connect( m_ui->checkBox,&QCheckBox::stateChanged,[]( int s ){

		utility::showWarningOnExtendingCoverFile( s == Qt::Unchecked ) ;
	} ) ;

	this->show() ;
}

warnWhenExtendingContainerFile::~warnWhenExtendingContainerFile()
{
	delete m_ui ;
}

void warnWhenExtendingContainerFile::Hide()
{
	this->hide() ;
	m_function() ;
	this->deleteLater() ;
}

void warnWhenExtendingContainerFile::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->Hide() ;
}
