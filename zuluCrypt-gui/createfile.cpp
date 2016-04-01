/*
 *
 *  Copyright (c) 2011-2015
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

#include "ui_createfile.h"
#include "createfile.h"
#include "utility.h"
#include "../zuluCrypt-cli/constants.h"

#include "dialogmsg.h"

#include <QFileDialog>
#include <QFile>

#include <QProcess>
#include <QTimer>
#include <QCloseEvent>
#include <QMessageBox>

createfile::createfile( QWidget * parent,std::function< void( const QString& ) > f ) :
	QDialog( parent ),m_ui( new Ui::createfile ),m_function( std::move( f ) )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->progressBar->setMinimum( 0 ) ;
	m_ui->progressBar->setMaximum( 100 ) ;
	m_ui->progressBar->setValue( 0 ) ;

	m_ui->pbOpenFolder->setIcon( QIcon( ":/folder.png" ) ) ;

	connect( m_ui->checkBoxNoRandomData,SIGNAL( stateChanged( int ) ),this,SLOT( warnAboutRandomData( int ) ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) )  ;
	connect( m_ui->pbOpenFolder,SIGNAL( clicked() ),this,SLOT(pbOpenFolder() ) ) ;
	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreate() ) ) ;
	connect( m_ui->lineEditFileName,SIGNAL( textChanged( QString ) ),this,SLOT(fileTextChange( QString ) ) ) ;

	connect( this,SIGNAL( sendProgress( int ) ),this,SLOT( setProgress( int ) ) ) ;

	this->installEventFilter( this ) ;

	this->setWindowTitle( tr( "Create A Container File" ) ) ;

	m_running = false ;

	this->showUI() ;
}

bool createfile::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->pbCancel() ; } ) ;
}

void createfile::fileTextChange( QString txt )
{
	auto p = m_ui->lineEditFilePath->text() ;

	if( p.isEmpty() ){

		auto x = utility::homePath() + "/" + txt.split( "/" ).last() ;
		m_ui->lineEditFilePath->setText( x ) ;
		return ;
	}

	int i = p.lastIndexOf( "/" ) ;

	if( i != -1 ){

		p = p.mid( 0,i ) + "/" + txt.split( "/" ).last() ;

		m_ui->lineEditFilePath->setText( p ) ;
	}
}

void createfile::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void createfile::enableAll()
{
	m_ui->lineEditFileName->setEnabled( true ) ;
	m_ui->lineEditFilePath->setEnabled( true ) ;
	m_ui->lineEditFileSize->setEnabled( true ) ;
	m_ui->pbOpenFolder->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label_3->setEnabled( true ) ;
	m_ui->label_4->setEnabled( true ) ;
	m_ui->pbCreate->setEnabled( true ) ;
	m_ui->checkBoxNoRandomData->setEnabled( true ) ;
}

void createfile::disableAll()
{
	m_ui->pbCreate->setEnabled( false ) ;
	m_ui->lineEditFileName->setEnabled( false ) ;
	m_ui->lineEditFilePath->setEnabled( false ) ;
	m_ui->lineEditFileSize->setEnabled( false ) ;
	m_ui->comboBox->setEnabled( false ) ;
	m_ui->pbOpenFolder->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label_3->setEnabled( false ) ;
	m_ui->label_4->setEnabled( false ) ;
	m_ui->checkBoxNoRandomData->setEnabled( false ) ;
}


void createfile::showUI()
{
	this->enableAll() ;

	m_ui->comboBox->setCurrentIndex( 1 ) ;
	m_ui->lineEditFileName->clear() ;
	m_ui->lineEditFilePath->setText( utility::homePath() + "/" ) ;
	m_ui->lineEditFileSize->clear() ;
	m_ui->progressBar->setValue( 0 ) ;
	m_ui->lineEditFileName->setFocus() ;

	this->show() ;
}

void createfile::warnAboutRandomData( int e )
{
	if( e == Qt::Checked ){

		DialogMsg msg( this ) ;

		auto m = tr( "\nBy default,zuluCrypt creates a volume in a container file over randomly generated data \
to hide usage patterns of the container.\n\
\n\
This process takes time and it may take a very,very \
long time if the volume about to be created is large enough and this option exists to \
skip the process for the impatient among us but but it comes at a cost and the cost may be \
too high when it finally reveal itself while infront of an adversary when they look at \
the encrypted container and manage to derive meaning based on how the container looks from outside.\n\
\n\
If you know what you are doing,then continue by all means,if in doubt,my advise is to endure the \
process and be safer in the long run." ) ;

		msg.ShowUIInfo( tr( "INFO" ),m ) ;
	}
}

void createfile::pbCreate()
{
	DialogMsg msg( this ) ;

	auto fileName = m_ui->lineEditFileName->text() ;
	auto filePath = m_ui->lineEditFilePath->text() ;
	auto fileSize = m_ui->lineEditFileSize->text() ;

	if( fileName.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "File name field is empty" ) ) ;
	}
	if( filePath.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "File path field is empty" ) ) ;
	}
	if( fileSize.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "File size field is empty" ) ) ;
	}
	if( m_ui->checkBoxNoRandomData->isChecked() ){

		int e = msg.ShowUIYesNoDefaultNo( tr( "WARNING" ),tr( "Are you really sure you do not want to create a more secured volume?" ) ) ;

		if( e != QMessageBox::Yes ){

			return ;
		}
	}

	bool test ;

	fileSize.toInt( &test ) ;

	if( test == false ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Illegal character in the file size field.Only digits are allowed" ) ) ;
	}

	if( utility::pathExists( filePath ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "File with the same name and at the destination folder already exist" ) ) ;
	}
	if( !utility::canCreateFile( filePath ) ){
		msg.ShowUIOK( tr( "ERROR!" ),tr( "You dont seem to have writing access to the destination folder" ) ) ;
		m_ui->lineEditFilePath->setFocus() ;
		return ;
	}

	qulonglong size = 0 ;

	switch( m_ui ->comboBox->currentIndex() ){
	case 0 :size = fileSize.toULongLong() * 1024 ;
		break ;
	case 1 :size = fileSize.toULongLong() * 1024 * 1024 ;
		break ;
	case 2 :size = fileSize.toULongLong() * 1024 * 1024  * 1024 ;
		break ;
	}

	if( size < 3145728 ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Container file must be bigger than 3MB" ) ) ;
	}

	this->disableAll() ;

	QFile file( filePath ) ;

	if( !file.open( QIODevice::WriteOnly ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to create volume file" ) ) ;
	}

	emit sendProgress( 0 ) ;

	if( !file.resize( size ) ){
		QFile::remove( filePath ) ;
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to create volume file" ) ) ;
	}

	utility::changeFileOwner( file ) ;

	file.close() ;

	m_exit = false ;
	m_running = true ;

	if( m_ui->checkBoxNoRandomData->isChecked() ){

		m_function( filePath ) ;

	}else{
		int r = utility::clearVolume( filePath,&m_exit,0,[ this ]( int i ){ emit sendProgress( i ) ; } ).await() ;

		if( r == 5 ){

			msg.ShowUIOK( tr( "ERROR!" ),tr( "Operation terminated per user choice" ) ) ;
			QFile::remove( filePath ) ;

		}else if( r == 0 ){

			m_function( filePath ) ;

		}else{
			msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open cryptographic back end to generate random data" ) ) ;
			QFile::remove( filePath ) ;
		}
	}

	m_running = false ;

	this->HideUI() ;
}

void createfile::pbCancel()
{
	if( m_running ){

		auto x = tr( "Terminating file creation process" ) ;
		auto y = tr( "Are you sure you want to stop file creation process?" ) ;

		DialogMsg msg( this ) ;

		if( msg.ShowUIYesNoDefaultNo( x,y ) == QMessageBox::Yes ){

			m_exit = true ;
		}
	}else{
		this->HideUI() ;
	}
}

void createfile::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void createfile::setProgress( int p )
{
	m_ui->progressBar->setValue( p ) ;
}

void createfile::pbOpenFolder()
{
	auto p = tr( "Select Path to where the file will be created" ) ;
	auto q = utility::homePath() ;
	auto Z = QFileDialog::getExistingDirectory( this,p,q,QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){

		Z = Z + "/" + m_ui->lineEditFilePath->text().split( "/" ).last() ;
		m_ui->lineEditFilePath->setText( Z ) ;
	}
}

createfile::~createfile()
{
	delete m_ui ;
}
