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
#include "bin_path.h"
#include "dialogmsg.h"

#include <QFileDialog>

struct Unit{
	static const qint64 GB = 1024 * 1024 * 1024 ;
	static const qint64 MB = 1024 * 1024 ;
	static const qint64 KB = 1024 ;
};

static std::pair< qint64,QString > _offset( qint64 s,qint64 Unit,const char * unit )
{
	Task::await( [ & ](){

		while( s % Unit != 0 ){

			s++ ;
		}
	} ) ;

	auto m = s / Unit ;

	return { s,QString::number( m ) + unit } ;
}

static std::pair< qint64,QString > _volumeOffset( qint64 s )
{
	if( s >= Unit::GB ){

		return _offset( s,Unit::GB,"g" ) ;

	}else if( s >= Unit::MB ){

		return _offset( s,Unit::MB,"m" ) ;

	}else if( s >= Unit::KB ){

		return _offset( s,Unit::KB,"k" ) ;
	}else{
		return { s,QString::number( s ) + "b" } ;
	}
}

class createHiddenVolume
{
public:
	enum class openStatus{ FailedToGetRandomData,FailedToOpenDevice,Success } ;

	createHiddenVolume( const QString& file ) :
		m_file( file ),
		m_randomData( utility::RandomDataSource::get( utility::RandomDataSource::types::urandom ) )
	{
	}
	openStatus open()
	{
		if( m_randomData->open() ){

			m_file.open( QIODevice::WriteOnly | QIODevice::Append ) ;

			if( m_file.isOpen() ){

				return openStatus::Success ;
			}else{
				return openStatus::FailedToOpenDevice ;
			}
		}else{
			return openStatus::FailedToGetRandomData ;
		}
	}
	template< typename Function >
	bool resize( qint64 containerSize,qint64 offSet,Function function )
	{
		std::array< char,1024 > buffer ;

		qint64 size_written = 0 ;

		containerSize = containerSize + offSet - m_file.size() ;

		utility::progress update( 1500,std::move( function ) ) ;

		auto pgr = update.updater_qint() ;

		Task::await( [ & ](){

			while( true ){

				m_randomData->getData( buffer.data(),buffer.size() ) ;

				auto s = m_file.write( buffer.data(),buffer.size() ) ;

				if( s == -1 ){

					//WTF!!
				}

				m_file.flush() ;

				size_written += s ;

				pgr( containerSize,size_written ) ;

				/*
				 * This simple test makes sure the end result is atleast as big as we expect it
				 * to be. It may be bigger and this is a feature since the final size may not
				 * be a multiple of 512.
				 */
				if( size_written >= containerSize ){

					break ;
				}
			}
		} ) ;

		m_file.close() ;
		return true ;
	}
	bool createVolume( const QString& key,
			   const QString& volumePath,
			   const QString& enc,
			   const QString& offset )
	{
		auto path = volumePath ;
		path.replace( "\"","\"\"\"" ) ;

		auto keyPath = utility::keyPath() ;

		utility::keySend( keyPath,key ) ;

		QString exe = "%1 -c -k -d \"%2\" -z ext4 -t plain -f %3  -g /dev/urandom.%4.%5b" ;
		exe = exe.arg( ZULUCRYPTzuluCrypt,path,keyPath,enc,offset ) ;

		return utility::Task::run( utility::appendUserUID( exe ) ).await().success() ;
	}
private:
	QFile m_file ;
	std::unique_ptr< utility::RandomDataSource > m_randomData ;
};

createVolumeInExistingFIle::createVolumeInExistingFIle( QWidget * parent ) :
	QDialog( parent ),
	m_ui( new Ui::createVolumeInExistingFIle )
{
	m_ui->setupUi( this ) ;

	m_ui->lineEditPassword->setMaxLength( 32767 ) ;

	m_warning = tr( "Extending A Host File Size." ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->pbSetFile->setIcon( QIcon( ":/file.png" ) ) ;

	this->displayWarning( false ) ;

	connect( m_ui->cbShowPassword,&QCheckBox::stateChanged,[ this ]( int s ){

		if( s == Qt::Checked ){

			m_ui->lineEditPassword->setEchoMode( QLineEdit::Normal ) ;
		}else{
			m_ui->lineEditPassword->setEchoMode( QLineEdit::Password ) ;
		}
	} ) ;

	connect( m_ui->pbSetFile,&QPushButton::clicked,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Enter Path To Existing File" ),utility::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditVolumePath->setText( e ) ;

			this->disableAll() ;

			m_volumeOffset = _volumeOffset( QFile( e ).size() ) ;

			m_ui->lineEditVolumeOffset->setText( m_volumeOffset.second ) ;

			this->enableAll() ;
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

void createVolumeInExistingFIle::updateProgress( QString cs,QString av,QString eta,QString tt,int s )
{
	Q_UNUSED( cs )
	Q_UNUSED( tt )

	QString a = tr( "Average Speed:" ) + " " + av + "\n" ;
	QString b = tr( "ETA:" ) + " " + eta + "\n" ;
	QString c = tr( "Percentage Completed: %1%" ).arg( QString::number( s ) ) ;

	m_ui->labelWarning->setText( m_warning + "\n\n" + a + b + c ) ;
}

void createVolumeInExistingFIle::enableAll()
{
	m_ui->frame->setEnabled( true ) ;
}

void createVolumeInExistingFIle::disableAll()
{
	m_ui->frame->setEnabled( false ) ;
}

void createVolumeInExistingFIle::displayWarning( bool e )
{
	m_ui->labelWarning->setVisible( e ) ;
}

void createVolumeInExistingFIle::create()
{
	auto volumePath = m_ui->lineEditVolumePath->text() ;
	auto containersize = m_ui->lineEditContainerSize->text() ;
	auto key = m_ui->lineEditPassword->text() ;
	auto volumeOffset = m_ui->lineEditVolumeOffset->text() ;

	if( volumePath.isEmpty() || containersize.isEmpty() || key.isEmpty() || volumeOffset.isEmpty() ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "AtLeast One Required Field Is Empty" ) ) ;
	}

	createHiddenVolume volume( volumePath ) ;

	bool ok ;

	auto containerSize = containersize.toLongLong( &ok ) * 1024 * 1024 ;

	if( !ok ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Illegal Character Found In The Container Size Field" ) ) ;
	}

	auto status = volume.open() ;

	if( status == createHiddenVolume::openStatus::FailedToOpenDevice ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Open File In Write Mode" ) ) ;

	}else if( status == createHiddenVolume::openStatus::FailedToGetRandomData ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Open \"/dev/urandom\" Device In Read Mode" ) ) ;
	}

	this->displayWarning( true ) ;

	volume.resize( containerSize,m_volumeOffset.first,[ this ]( const utility::progress::result& m ){

		QMetaObject::invokeMethod( this,
					   "updateProgress",
					   Q_ARG( QString,m.current_speed ),
					   Q_ARG( QString,m.average_speed ),
					   Q_ARG( QString,m.eta ),
					   Q_ARG( QString,m.total_time ),
					   Q_ARG( int,m.percentage_done ) ) ;
	} ) ;

	auto enc = m_ui->cbVolumeProperties->currentText() ;
	auto offSet = QString::number( m_volumeOffset.first ) ;

	m_ui->labelWarning->setText( tr( "Creating A Plain DM-Crypt Volume" ) ) ;

	if( volume.createVolume( key,volumePath,enc,offSet ) ){

		m_ui->labelWarning->setText( tr( "Volume Created Successfully" ) ) ;
	}else{
		m_ui->labelWarning->setText( tr( "Failed To Create A Volume" ) ) ;
	}

	utility::Task::suspend( 2 ) ;

	this->Hide() ;
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

bool createVolumeInExistingFIle::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->Hide() ; } ) ;
}
