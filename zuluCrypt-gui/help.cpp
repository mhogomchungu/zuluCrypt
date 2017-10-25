/*
 *
 *  Copyright ( c ) 2015
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

#include "help.h"
#include "ui_help.h"

#include <QObject>
#include <QCloseEvent>
#include <QEvent>

#include "utility.h"

#include "pdf_path.h"

help::help( QWidget * parent,const QString& path ) :
	QDialog( parent ),
	m_ui( new Ui::help ),
	m_openPath( path )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	this->setFont( parent->font() ) ;

	connect( m_ui->pushButton,SIGNAL( clicked( bool ) ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->pushButton_2,SIGNAL( clicked( bool ) ),this,SLOT( pbOpenPDF() ) ) ;

	this->show() ;
}

help::~help()
{
	delete m_ui ;
}

void help::pbClose()
{
	this->hide() ;
	this->deleteLater() ;
}

void help::pbOpenPDF()
{
	auto x = tr( "WARNING!" ) ;
	auto y = tr( "Failed to open zuluCrypt.pdf,make sure your system can open pdf files using \"%1\" tool and try again" ).arg( m_openPath ) ;

	QString e = PDF_PATH ;

	if( utility::pathExists( e ) ){

		utility::openPath( e,m_openPath,this,x,y ) ;
	}else{
		e += ".gz" ;

		if( utility::pathExists( e ) ){

			utility::openPath( e,m_openPath,this,x,y ) ;
		}else{
			utility::openPath( PDF_PATH,m_openPath,this,x,y ) ;
		}
	}
}

void help::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbClose() ;
}

bool help::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->pbClose() ; } ) ;
}
