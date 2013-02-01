#include "keydialog.h"
#include "ui_keydialog.h"

keyDialog::keyDialog( QWidget * parent,QTableWidget * table,QString path,QString type,QString folderOpener ) :
	QDialog( parent ),m_ui(new Ui::keyDialog)
{
	m_ui->setupUi(this);
	m_table = table ;
	m_path = path ;
	m_working = false ;
	m_folderOpener = folderOpener ;

	QString msg ;
	if( type == QString( "crypto_LUKS" ) )
		msg = tr( "unlock and mount a luks volume in \"%1\"").arg( m_path ) ;
	else
		msg = tr( "unlock and mount a plain volume in \"%1\"").arg( m_path ) ;

	this->setWindowTitle( msg );

	path = savemountpointpath::getPath( path,QString( "zuluMount-MountPointPath" ) ) ;

	m_ui->lineEditMountPoint->setText( path );
	m_ui->pbkeyFile->setVisible( false );
	m_ui->pbOpenMountPoint->setIcon( QIcon( QString( ":/folder.png" ) ) );

	m_menu = new QMenu( this ) ;

	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );
	this->setFont( parent->font() );

	m_ui->lineEditKey->setFocus();

	m_ui->checkBoxOpenReadOnly->setChecked( openvolumereadonly::getOption( QString( "zuluMount-gui" ) ) );

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbkeyOption,SIGNAL( clicked() ),this,SLOT( pbkeyOption() ) ) ;
	connect( m_ui->rbKey,SIGNAL( toggled( bool ) ),this,SLOT( rbKey( bool ) ) ) ;
	connect( m_ui->rbKeyFile,SIGNAL( toggled( bool ) ),this,SLOT( rbKeyFile( bool ) ) ) ;
	connect( m_ui->rbPlugIn,SIGNAL( toggled( bool ) ),this,SLOT( rbPlugIn( bool ) ) ) ;
	connect( m_ui->lineEditKey,SIGNAL( textChanged( QString ) ),this,SLOT( keyTextChanged( QString ) ) ) ;
	connect( m_ui->pbOpenMountPoint,SIGNAL( clicked() ),this,SLOT( pbMountPointPath() ) ) ;
	connect( m_ui->checkBoxOpenReadOnly,SIGNAL( stateChanged( int ) ),this,SLOT( cbMountReadOnlyStateChanged( int ) ) ) ;
	m_ui->rbKey->setChecked( true ) ;

	m_ui->pbOpenMountPoint->setVisible( false );

	m_point = m_path.split( "/" ).last() ;
	m_ui->lineEditMountPoint->setText( m_point );
}

void keyDialog::cbMountReadOnlyStateChanged( int state )
{
	m_ui->checkBoxOpenReadOnly->setEnabled( false );
	m_ui->checkBoxOpenReadOnly->setChecked( openvolumereadonly::setOption( this,state,QString( "zuluMount-gui" ) ) );
	m_ui->checkBoxOpenReadOnly->setEnabled( true );
	if( m_ui->lineEditKey->text().isEmpty() )
		m_ui->lineEditKey->setFocus();
	else if( m_ui->lineEditMountPoint->text().isEmpty() )
		m_ui->lineEditMountPoint->setFocus();
	else
		m_ui->pbOpen->setFocus();
}

void keyDialog::keyTextChanged( QString txt )
{
	if( m_ui->rbPlugIn->isChecked() ){
		if( txt.contains( QString( "/") ) )
			m_ui->label->setText( tr( "plugin path" ) );
		else
			m_ui->label->setText( tr( "plugin name" ) );
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

	list.removeOne( QString( "zuluCrypt-testKey" ) ) ;
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
	MainWindow::volumeMiniProperties( m_table,prp,utility::mountPath( m_point ) ) ;
	this->HideUI();
}

void keyDialog::fileManagerOpenStatus( int exitCode, int exitStatus,int startError )
{
	Q_UNUSED( startError ) ;
	if( exitCode != 0 || exitStatus != 0 ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "warning" ),tr( "could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) );
	}
}

void keyDialog::slotMountComplete( int st,QString m )
{
	m_working = false ;
	if( st == 0 ){
		if( utility::mapperPathExists( m_path ) ) {
			/*
			 * The volume is reported as opened and it actually is
			 */
			managepartitionthread * mpt = new managepartitionthread() ;
			mpt->setDevice( m_path );
			connect( mpt,SIGNAL( signalProperties( QString ) ),this,SLOT( volumeMiniProperties( QString ) ) ) ;
			mpt->startAction( QString( "volumeMiniProperties" ) ) ;

			openmountpointinfilemanager * omp = new openmountpointinfilemanager( m_folderOpener,utility::mountPath( m_point ) ) ;
			connect( omp,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
			omp->start();
		}else{
			/*
			 * The volume is reported as opened but it isnt,possible reason is a backe end crash
			 */

			DialogMsg m( this ) ;

			m.ShowUIOK( QString( "ERROR" ),QString( "An error has occured and the volume could not be opened" ) );
			this->HideUI();
		}
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

	QString test_name = m_ui->lineEditMountPoint->text() ;
	if( test_name.contains( QString( "/" ) ) ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "\"/\" character is not allowed in the mount name field" ) ) ;
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
	m_point = m_ui->lineEditMountPoint->text().replace( "\"","\"\"\"" ) ;
	part->setMountPoint( m_point );

	m_working = true ;
	//savemountpointpath::savePath( m_ui->lineEditMountPoint->text(),QString( "zuluMount-MountPointPath" ) ) ;

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
