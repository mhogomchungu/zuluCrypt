#include "keydialog.h"
#include "ui_keydialog.h"

keyDialog::keyDialog( QWidget * parent,QTableWidget * table,QString path ) :
	QDialog( parent ),m_ui(new Ui::keyDialog)
{
	m_ui->setupUi(this);
	m_table = table ;
	m_path = path ;
	m_working = false ;

	QString msg = tr( "unlock and mount a luks volume in \"%1\"").arg( m_path ) ;

	this->setWindowTitle( msg );

	m_ui->lineEditMountPoint->setText( QDir::homePath() + QString( "/" ) + m_path.split( "/" ).last() );
	//m_ui->pbkeyOption->setIcon( QIcon( QString( ":/module.png" ) ) );
	//m_ui->pbkeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );
	m_ui->pbkeyFile->setVisible( false );
	m_ui->pbOpenMountPoint->setIcon( QIcon( QString( ":/folder.png" ) ) );

	m_menu = new QMenu( this ) ;

	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );
	this->setFont( parent->font() );

	m_ui->lineEditKey->setFocus();

	m_ui->checkBoxOpenReadOnly->setChecked( openvolumereadonly::getOption() );

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	//connect( m_ui->pbPlugin,SIGNAL( clicked() ),this,SLOT( pbPlugin() ) ) ;
	//connect( m_ui->pbkeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;
	connect( m_ui->pbkeyOption,SIGNAL( clicked() ),this,SLOT( pbkeyOption() ) ) ;
	connect( m_ui->rbKey,SIGNAL( toggled( bool ) ),this,SLOT( rbKey( bool ) ) ) ;
	connect( m_ui->rbKeyFile,SIGNAL( toggled( bool ) ),this,SLOT( rbKeyFile( bool ) ) ) ;
	connect( m_ui->rbPlugIn,SIGNAL( toggled( bool ) ),this,SLOT( rbPlugIn( bool ) ) ) ;
	connect( m_ui->lineEditKey,SIGNAL( textChanged( QString ) ),this,SLOT( keyTextChanged( QString ) ) ) ;
	connect( m_ui->pbOpenMountPoint,SIGNAL( clicked() ),this,SLOT( pbMountPointPath() ) ) ;
	connect( m_ui->checkBoxOpenReadOnly,SIGNAL( stateChanged( int ) ),this,SLOT( cbMountReadOnlyStateChanged( int ) ) ) ;
	m_ui->rbKey->setChecked( true ) ;
}

void keyDialog::cbMountReadOnlyStateChanged( int state )
{
	m_ui->checkBoxOpenReadOnly->setEnabled( false );
	m_ui->checkBoxOpenReadOnly->setChecked( openvolumereadonly::setOption( this,state ) );
	m_ui->checkBoxOpenReadOnly->setEnabled( true );
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
	m_ui->pbkeyOption->setEnabled( true );
	m_ui->checkBoxOpenReadOnly->setEnabled( true );
}

void keyDialog::disableAll()
{
	m_ui->pbkeyOption->setEnabled( false );
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
	m_ui->checkBoxOpenReadOnly->setEnabled( false );
}

void keyDialog::KeyFile()
{
	if( m_ui->rbKeyFile->isChecked() ){
		QString msg = tr( "select a file to be used as a keyfile" ) ;
		QString Z = QFileDialog::getOpenFileName( this,msg,QDir::homePath() ) ;

		if( !Z.isEmpty() ){
			m_ui->lineEditKey->setText( Z );
		}
	}
}

void keyDialog::pbkeyOption()
{
	if( m_ui->rbPlugIn->isChecked() )
		this->Plugin();
	else if( m_ui->rbKeyFile->isChecked() )
		this->KeyFile();
}

void keyDialog::Plugin()
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

void keyDialog::volumeMiniProperties( QString prp )
{
	MainWindow::volumeMiniProperties( m_table,prp,m_ui->lineEditMountPoint->text() ) ;
	this->HideUI();
}

void keyDialog::slotMountComplete( int st,QString m )
{
	m_working = false ;
	if( st == 0 ){

		managepartitionthread * mpt = new managepartitionthread() ;
		mpt->setDevice( m_table->item( m_table->currentRow(),0 )->text() );
		connect( mpt,SIGNAL( signalProperties( QString ) ),this,SLOT( volumeMiniProperties( QString ) ) ) ;
		mpt->startAction( QString( "volumeMiniProperties" ) ) ;
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
	m_working = true ;

	if( m_ui->lineEditKey->text().isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "passphrase field is empty" ) ) ;
		m_ui->lineEditKey->setFocus();
		return ;
	}

	QString m ;
	if( m_ui->rbKey->isChecked() ){
		QString addr = socketSendKey::getSocketPath() ;
		m = QString( "-f ") + addr ;

		socketSendKey * s = new socketSendKey( this,addr,m_ui->lineEditKey->text().toAscii() ) ;
		s->sendKey();
	}else if( m_ui->rbKeyFile->isChecked() )
		m = QString( "-f ") + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;
	else if( m_ui->rbPlugIn->isChecked() )
		m = QString( "-G ") + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;

	managepartitionthread * part = new managepartitionthread() ;
	connect( part,SIGNAL( signalMountComplete( int,QString ) ),this,SLOT( slotMountComplete( int,QString ) ) ) ;

	part->setDevice( m_path );
	if( m_ui->checkBoxOpenReadOnly->isChecked() )
		part->setMode( QString( "ro" ) );
	else
		part->setMode( QString( "rw" )) ;
	part->setKeySource( m );
	part->setMountPoint( m_ui->lineEditMountPoint->text().replace( "\"","\"\"\"" ) );

	part->startAction( QString( "cryptoOpen" ) ) ;
	this->disableAll();
}

void keyDialog::rbPlugIn( bool opt )
{
	if( opt ){
		m_ui->pbkeyOption->setIcon( QIcon( QString( ":/module.png" ) ) );
		m_ui->lineEditKey->setEchoMode( QLineEdit::Normal );
		m_ui->label->setText( tr( "plugin name" ) );
		m_ui->lineEditKey->clear();
		m_ui->pbkeyOption->setEnabled( true );
	}
}

void keyDialog::rbKey( bool opt )
{
	if( opt ){
		m_ui->pbkeyOption->setIcon( QIcon( QString( ":/passphrase.png" ) ) );
		m_ui->pbkeyOption->setEnabled( false );
		m_ui->label->setText( tr( "key" ) );
		m_ui->lineEditKey->setEchoMode( QLineEdit::Password );
		m_ui->pbkeyFile->setEnabled( false );
		m_ui->lineEditKey->clear();
	}
}

void keyDialog::rbKeyFile( bool opt )
{
	if( opt ){
		m_ui->pbkeyOption->setIcon( QIcon( QString( ":/keyfile.png" ) ) );
		m_ui->lineEditKey->setEchoMode( QLineEdit::Normal );
		m_ui->label->setText( tr( "keyfile path" ) );
		m_ui->pbkeyOption->setEnabled( true );
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
	if( !m_working ){
		emit hideUISignal();
		this->hide();
	}
}

keyDialog::~keyDialog()
{
	m_menu->deleteLater();
	delete m_ui;
}
