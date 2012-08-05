#include "keydialog.h"
#include "ui_keydialog.h"

keyDialog::keyDialog( QWidget * parent,QString path,QString mode ) :
	QDialog( parent ),
	m_ui(new Ui::keyDialog)
{
	m_ui->setupUi(this);

	m_path = path ;
	m_mode = mode ;

	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );
	this->setFont( parent->font() );

	m_ui->lineEditKey->setFocus();

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbOpenKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFileOpen() ) ) ;
	connect( m_ui->rbKey,SIGNAL( toggled( bool ) ),this,SLOT( rbKey( bool ) ) ) ;
	connect( m_ui->rbKeyFile,SIGNAL( toggled( bool ) ),this,SLOT( rbKeyFile( bool ) ) ) ;
	m_ui->rbKey->setChecked( true ) ;
}

void keyDialog::enableAll()
{
	m_ui->pbCancel->setEnabled( true );
	m_ui->pbOpen->setEnabled( true );
	m_ui->label->setEnabled( true );
	m_ui->rbKey->setEnabled( true );
	m_ui->rbKeyFile->setEnabled( true );
	m_ui->lineEditKey->setEnabled( true );

	if( m_ui->rbKeyFile->isChecked() )
		m_ui->pbOpenKeyFile->setEnabled( true );
}

void keyDialog::disableAll()
{
	m_ui->lineEditKey->setEnabled( false );
	m_ui->pbCancel->setEnabled( false );
	m_ui->pbOpen->setEnabled( false );
	m_ui->pbOpenKeyFile->setEnabled( false );
	m_ui->label->setEnabled( false );
	m_ui->rbKey->setEnabled( false );
	m_ui->rbKeyFile->setEnabled( false );
}

void keyDialog::pbKeyFileOpen()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "keyfile path" ),QDir::homePath(),0 );

	if( !Z.isEmpty() )
		m_ui->lineEditKey->setText( Z );
}

void keyDialog::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->HideUI();
}

void keyDialog::slotMountComplete( int st,QString m )
{
	if( st == 0 ){
		emit updatePartitions();
		this->HideUI();
	}else{
		DialogMsg msg( this ) ;

		msg.ShowUIOK( QString( "ERROR" ),m ) ;
		this->enableAll();
	}
}

void keyDialog::pbOpen()
{
	QString m ;
	if( m_ui->rbKey->isChecked() )
		m = QString( "-p ") + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;
	else
		m = QString( "-f ") + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;

	managepartitionthread * part = new managepartitionthread() ;
	connect( part,SIGNAL( signalMountComplete( int,QString ) ),this,SLOT( slotMountComplete( int,QString ) ) ) ;

	part->setDevice( m_path );
	part->setMode( m_mode );
	part->setKeySource( m );

	part->startAction( QString( "cryptoOpen" ) ) ;
	this->disableAll();
}

void keyDialog::rbKey( bool opt)
{
	if( opt ){
		m_ui->label->setText( QString( "key: " ) );
		m_ui->lineEditKey->setEchoMode( QLineEdit::Password );
		m_ui->pbOpenKeyFile->setIcon( QIcon( QString( ":/passphrase.png" ) ) );
		m_ui->pbOpenKeyFile->setEnabled( false );
		m_ui->lineEditKey->clear();
	}
}

void keyDialog::rbKeyFile( bool opt )
{
	if( opt ){
		m_ui->lineEditKey->setEchoMode( QLineEdit::Normal );
		m_ui->label->setText( QString( "key from keyfile: " ) );
		m_ui->pbOpenKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );
		m_ui->pbOpenKeyFile->setEnabled( true );
		m_ui->lineEditKey->clear();
	}
}

void keyDialog::pbCancel()
{
	this->HideUI();
}

void keyDialog::ShowUI()
{
	this->show();
}

void keyDialog::HideUI()
{
	emit hideUISignal();
	this->hide();
}

keyDialog::~keyDialog()
{
	delete m_ui;
}
