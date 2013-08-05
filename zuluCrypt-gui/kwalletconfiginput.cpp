#include "kwalletconfiginput.h"
#include "ui_kwalletconfiginput.h"

kwalletconfiginput::kwalletconfiginput( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::kwalletconfiginput )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() );
	if( parent ){
		this->setFont( parent->font() );
	}

	m_ui->lineEditVolumeID->setEnabled( false ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
	m_ui->lineEditRepeatKey->setEchoMode( QLineEdit::Password ) ;

	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( HideUI() ) ) ;
	connect( m_ui->pushButtonImageFile,SIGNAL( clicked() ),this,SLOT( pbImageFilePath() ) ) ;
	connect( m_ui->pushButtonVolume,SIGNAL( clicked() ),this,SLOT( pbVolumePath() ) ) ;

	m_ui->pushButtonImageFile->setIcon( QIcon( QString( ":/file.png" ) ) ) ;
	m_ui->pushButtonVolume->setIcon( QIcon( QString( ":/partition.png" ) ) ) ;
}

kwalletconfiginput::~kwalletconfiginput()
{
	delete m_ui ;
}

void kwalletconfiginput::ShowUI()
{
	this->show() ;
}

void kwalletconfiginput::HideUI()
{
	this->deleteLater() ;
}

void kwalletconfiginput::pbAdd()
{
	QString volumeID  = m_ui->lineEditVolumeID->text() ;
	QString comment   = m_ui->lineEditComment->text()  ;
	QString key       = m_ui->lineEditKey->text()      ;
	QString repeatKey = m_ui->lineEditRepeatKey->text();

	if( volumeID.isEmpty() || key.isEmpty() || repeatKey.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "atleast one required field is empty" ) ) ;
		return ;
	}

	if( key != repeatKey ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "passphrases do not match" ) ) ;
		return ;
	}

	if( comment.isEmpty() ){
		comment = QString( "Nil" ) ;
	}

	emit add( volumeID,comment,key ) ;
	this->HideUI() ;
}

void kwalletconfiginput::pbImageFilePath()
{
	QString x = QFileDialog::getOpenFileName( this,tr( "select a luks volume" ),QDir::homePath(),0 );

	if( x.isEmpty() ){
		return ;
	}

	QString z = utility::getUUIDFromPath( x ) ;

	if( z.isEmpty() ){
		this->setvolumeID( x ) ;
	}else{
		this->setvolumeID( z ) ;
	}
}

void kwalletconfiginput::pbVolumePath()
{
	openvolume * op = new openvolume( this ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( setvolumeID( QString ) ) ) ;
	op->showEncryptedOnly();
	op->ShowAllPartitions();
}

void kwalletconfiginput::setvolumeID( QString id )
{
	m_ui->lineEditVolumeID->setText( id ) ;
	this->focus() ;
}

void kwalletconfiginput::focus()
{
	if( m_ui->lineEditComment->text().isEmpty() ){
		m_ui->lineEditComment->setFocus() ;
	}else if( m_ui->lineEditKey->text().isEmpty() ){
		m_ui->lineEditKey->setFocus() ;
	}else if( m_ui->lineEditRepeatKey->text().isEmpty() ){
		m_ui->lineEditRepeatKey->setFocus() ;
	}else{
		m_ui->pushButtonAdd->setFocus() ;
	}
}

void kwalletconfiginput::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}
