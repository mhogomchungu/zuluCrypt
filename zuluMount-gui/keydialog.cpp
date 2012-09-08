#include "keydialog.h"
#include "ui_keydialog.h"

keyDialog::keyDialog( QWidget * parent,QString path,QString mode ) :
	QDialog( parent ),
	m_ui(new Ui::keyDialog)
{
	m_ui->setupUi(this);

	m_path = path ;
	m_mode = mode ;

	QString msg = tr( "unlock and mount a luks volume in \"%1\"").arg( m_path ) ;

	this->setWindowTitle( msg );

	m_ui->lineEditMountPoint->setText( QDir::homePath() + QString( "/" ) + m_path.split( "/" ).last() );
	m_ui->pbPlugin->setIcon( QIcon( QString( ":/module.png" ) ) );
	m_ui->pbkeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );
	m_ui->pbOpenMountPoint->setIcon( QIcon( QString( ":/folder.png" ) ) );

	m_menu = new QMenu( this ) ;

	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );
	this->setFont( parent->font() );

	m_ui->lineEditKey->setFocus();

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbPlugin,SIGNAL( clicked() ),this,SLOT( pbPlugin() ) ) ;
	connect( m_ui->rbKey,SIGNAL( toggled( bool ) ),this,SLOT( rbKey( bool ) ) ) ;
	connect( m_ui->rbKeyFile,SIGNAL( toggled( bool ) ),this,SLOT( rbKeyFile( bool ) ) ) ;
	connect( m_ui->rbPlugIn,SIGNAL( toggled( bool ) ),this,SLOT( rbPlugIn( bool ) ) ) ;
	connect( m_ui->lineEditKey,SIGNAL( textChanged( QString ) ),this,SLOT( keyTextChanged( QString ) ) ) ;
	connect( m_ui->pbOpenMountPoint,SIGNAL( clicked() ),this,SLOT( pbMountPointPath() ) ) ;
	connect( m_ui->pbkeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;
	m_ui->rbKey->setChecked( true ) ;
}

void keyDialog::keyTextChanged( QString txt )
{
	if( m_ui->rbPlugIn->isChecked() ){
		if( txt.contains( QString( "/") ) )
			m_ui->label->setText( tr( "plugin path") );
		else
			m_ui->label->setText( tr( "plugin name") );
	}
}

void keyDialog::pbMountPointPath()
{
	QString msg = tr( "select a folder to create a mount point in" ) ;
	QString Z = QFileDialog::getExistingDirectory( this,msg,QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){
		Z = Z + QString( "/" ) + m_ui->lineEditMountPoint->text().split( "/" ).last() ;
		m_ui->lineEditMountPoint->setText( Z );
	}
}

void keyDialog::enableAll()
{
	m_ui->label_2->setEnabled( true );
	m_ui->lineEditMountPoint->setEnabled( true );
	m_ui->pbOpenMountPoint->setEnabled( true );
	m_ui->pbCancel->setEnabled( true );
	m_ui->pbOpen->setEnabled( true );
	m_ui->label->setEnabled( true );
	m_ui->rbKey->setEnabled( true );
	m_ui->rbKeyFile->setEnabled( true );
	m_ui->lineEditKey->setEnabled( true );
	m_ui->rbPlugIn->setEnabled( true );
	if( m_ui->rbPlugIn->isChecked() )
		m_ui->pbPlugin->setEnabled( true );
	else if( m_ui->rbKeyFile->isChecked() )
		m_ui->pbkeyFile->setEnabled( true );
}

void keyDialog::disableAll()
{
	m_ui->label_2->setEnabled( false );
	m_ui->lineEditMountPoint->setEnabled( false );
	m_ui->pbOpenMountPoint->setEnabled( false );
	m_ui->lineEditKey->setEnabled( false );
	m_ui->pbCancel->setEnabled( false );
	m_ui->pbOpen->setEnabled( false );
	m_ui->pbkeyFile->setEnabled( false );
	m_ui->label->setEnabled( false );
	m_ui->rbKey->setEnabled( false );
	m_ui->rbKeyFile->setEnabled( false );
	m_ui->rbPlugIn->setEnabled( false );
}

void keyDialog::pbKeyFile()
{
	if( m_ui->rbKeyFile->isChecked() ){
		QString msg = tr( "select a file to be used as a keyfile" ) ;
		QString Z = QFileDialog::getOpenFileName( this,msg,QDir::homePath() ) ;

		if( !Z.isEmpty() ){
			m_ui->lineEditKey->setText( Z );
		}
	}
}

void keyDialog::pbPlugin()
{
	QStringList list ;

	// ZULUCRYPTpluginPath is set at config time and it equals $prefix/lib(64)/zuluCrypt

	QDir dir( QString( ZULUCRYPTpluginPath ) ) ;

	if( !dir.exists() )
		return ;

	list = dir.entryList() ;

	list.removeOne( QString( ".") ) ;
	list.removeOne( QString( "..") ) ;

	if( list.contains( QString( "kwallet" ) ) ){
		int index = list.indexOf( "kwallet" ) ;
		list.move( index,0 );
	}

	m_menu->clear();

	int j = list.size()  ;

	if( j == 0 )
		return ;

	for( int i = 0 ; i < j ; i++ )
		m_menu->addAction( list.at( i ) ) ;

	m_menu->addSeparator() ;

	m_menu->addAction( tr( "cancel" ) ) ;

	connect( m_menu,SIGNAL( triggered( QAction * ) ),this,SLOT( pbPluginEntryClicked( QAction * ) ) ) ;

	m_menu->exec( QCursor::pos() ) ;
}

void keyDialog::pbPluginEntryClicked( QAction * e )
{
	if( e->text() != tr( "cancel" ) )
		m_ui->lineEditKey->setText( e->text() ) ;
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
		m_ui->lineEditKey->clear();
		m_ui->rbKey->setChecked( true );
		this->enableAll();
		m_ui->lineEditKey->setFocus();
	}
}

void keyDialog::pbOpen()
{
	if( m_ui->lineEditKey->text().isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "passphrase field is empty" ) ) ;
		m_ui->lineEditKey->setFocus();
		return ;
	}

	QString m ;
	if( m_ui->rbKey->isChecked() ){
		QString addr = zuluOptions::getSocketPath() ;
		m = QString( "-f ") + addr ;
		zuluSocket * zs = new zuluSocket( this ) ;
		connect( zs,SIGNAL( gotConnected( zuluSocket * ) ),this,SLOT( sendKey( zuluSocket * ) ) ) ;
		connect( zs,SIGNAL( doneWritingData() ),zs,SLOT( deleteLater() ) ) ;
		zs->startServer( addr );
	}else if( m_ui->rbKeyFile->isChecked() )
		m = QString( "-f ") + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;
	else if( m_ui->rbPlugIn->isChecked() )
		m = QString( "-G ") + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;

	managepartitionthread * part = new managepartitionthread() ;
	connect( part,SIGNAL( signalMountComplete( int,QString ) ),this,SLOT( slotMountComplete( int,QString ) ) ) ;

	part->setDevice( m_path );
	part->setMode( m_mode );
	part->setKeySource( m );
	part->setMountPoint( m_ui->lineEditMountPoint->text().replace( "\"","\"\"\"" ) );

	part->startAction( QString( "cryptoOpen" ) ) ;
	this->disableAll();
}

void keyDialog::sendKey( zuluSocket * s )
{
	QByteArray data = m_ui->lineEditKey->text().toAscii() ;
	s->sendData( &data );
}

void keyDialog::rbPlugIn( bool opt )
{
	if( opt ){
		m_ui->lineEditKey->setEchoMode( QLineEdit::Normal );
		m_ui->label->setText( tr( "plugin name" ) );
		m_ui->pbPlugin->setEnabled( true );
		m_ui->pbkeyFile->setEnabled( false );
		m_ui->lineEditKey->clear();
	}
}

void keyDialog::rbKey( bool opt )
{
	if( opt ){
		m_ui->label->setText( tr( "key" ) );
		m_ui->lineEditKey->setEchoMode( QLineEdit::Password );
		m_ui->pbPlugin->setEnabled( false );
		m_ui->pbkeyFile->setEnabled( false );
		m_ui->lineEditKey->clear();
	}
}

void keyDialog::rbKeyFile( bool opt )
{
	if( opt ){
		m_ui->lineEditKey->setEchoMode( QLineEdit::Normal );
		m_ui->label->setText( tr( "keyfile path" ) );
		m_ui->pbkeyFile->setEnabled( true );
		m_ui->pbPlugin->setEnabled( false );
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
	m_menu->deleteLater();
	delete m_ui;
}
