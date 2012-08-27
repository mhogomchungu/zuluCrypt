#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget * parent ) : QMainWindow( parent ),m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this );

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password );

	m_client = 0 ;
	m_server = 0 ;

	this->setWindowIcon( QIcon( QString( ":/keyfile.png" ) ) );
	m_ui->pbKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );

	m_server = new QLocalServer( this ) ;
	connect( m_server,SIGNAL( newConnection() ),this,SLOT( acceptConnection() ) ) ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	this->SetFocus();
	m_ui->pbOpen->setEnabled( false );
}

void MainWindow::SetAddr( QString addr )
{
	m_addr = addr ;
	m_server->listen( m_addr ) ;
}

void MainWindow::acceptConnection()
{
	m_client = m_server->nextPendingConnection() ;
	m_ui->pbOpen->setEnabled( true );
}

void MainWindow::SetFocus()
{
	if( m_ui->lineEditKey->text().isEmpty() )
		m_ui->lineEditKey->setFocus();
	else if( m_ui->lineEditKeyFile->text().isEmpty() )
		m_ui->lineEditKeyFile->setFocus();
	else
		m_ui->pbOpen->setFocus();
}

void MainWindow::pbCancel()
{
	this->Exit();
}

void MainWindow::Exit()
{
	if( m_client ){
		if( m_client->isOpen() )
			m_client->close();

		m_client->deleteLater();
	}

	if( m_server ){
		m_server->close();
		m_server->deleteLater();
	}

	QFile::remove( m_addr ) ;

	this->hide();

	QCoreApplication::quit() ;
}

void MainWindow::pbOpen()
{
	QString key = m_ui->lineEditKey->text() ;

	m_keyFile = m_ui->lineEditKeyFile->text() ;

	if( !m_keyFile.isEmpty() ){

		QString kar = m_keyFile.mid( 0,1 ) ;

		if( kar == QString( "~" ) )
			m_keyFile = QDir::homePath() + QString( "/" ) + m_keyFile.mid( 1 ) ;

		if( kar != QString( "/" ) )
			m_keyFile = QDir::homePath() + QString( "/" ) + m_keyFile ;

		QFile file( m_keyFile ) ;

		if( file.open( QIODevice::ReadOnly ) ){
			key = key + QString( file.readAll() ) ;
			file.close();
		}
	}

	m_client->write( key.toAscii() ) ;
	m_client->flush() ;
	m_client->close();

	this->Exit();
}

void MainWindow::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,QString( "select a key file" ),QDir::homePath() ) ;

	if( !Z.isEmpty() )
		m_ui->lineEditKeyFile->setText( Z );
	this->SetFocus();
}

MainWindow::~MainWindow()
{
	delete m_ui;
}
