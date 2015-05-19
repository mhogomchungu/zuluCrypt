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

#include "readonlywarning.h"
#include "ui_readonlywarning.h"

#include "utility.h"

#include <QDir>

#include <QCloseEvent>
#include <QEvent>

static QString _configPathReadOnly( const QString& app )
{
	return QDir::homePath() + "/.zuluCrypt/" + app + "-readOnlyOption" ;
}

static QString _configPathShowUI( const QString& app )
{
	return QDir::homePath() + "/.zuluCrypt/" + app + "-readOnlyUIWarningOption" ;
}

bool readOnlyWarning::getOpenVolumeReadOnlyOption( const QString& app )
{
	QFile f( _configPathReadOnly( app ) ) ;

	if( f.exists() ){

		f.open( QIODevice::ReadOnly ) ;

		char e = '0' ;
		f.getChar( &e ) ;

		return e == '1' ;
	}else{
		return false ;
	}
}

readOnlyWarning::readOnlyWarning( QWidget * parent,bool checked,const QString& app ) :
	QDialog( parent ),m_ui( new Ui::readOnlyWarning ),m_checked( checked ),m_app( app )
{
	m_ui->setupUi( this ) ;

	this->setFont( parent->font() ) ;
	this->setFixedSize( this->size() ) ;

	connect( m_ui->PbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->checkBox,SIGNAL( clicked( bool ) ),this,SLOT( checkBoxChecked( bool ) ) ) ;

	this->installEventFilter( this ) ;

	m_configPathReadOnly = _configPathReadOnly( app ) ;
	m_configPathShowUI   = _configPathShowUI( app ) ;

	this->setReadOnlyOption( m_checked ) ;
}

void readOnlyWarning::pbOK()
{
	this->HideUI() ;
}

void readOnlyWarning::checkBoxChecked( bool checked )
{
	QFile f( m_configPathShowUI ) ;

	if( checked ){

		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	}else{
		f.remove() ;
	}
}

void readOnlyWarning::setReadOnlyOption( bool readOnly )
{
	QFile f( m_configPathReadOnly ) ;

	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;

	if( readOnly ){

		f.write( "1" ) ;
	}else{
		f.write( "0" ) ;
	}
}

bool readOnlyWarning::showUIwarning()
{
	return !utility::pathExists( m_configPathShowUI ) ;
}

bool readOnlyWarning::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){

		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void readOnlyWarning::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void readOnlyWarning::ShowUI()
{
	if( m_checked && this->showUIwarning() ){

		this->show() ;
	}else{
		this->deleteLater() ;
	}
}

void readOnlyWarning::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

readOnlyWarning::~readOnlyWarning()
{
	delete m_ui ;
}
