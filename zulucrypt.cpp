/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#include "version.h"
#include "zulucrypt.h"
#include "ui_zulucrypt.h"

#include <QProcess>
#include <QStringList>
#include <QMenu>
#include <QCursor>
#include <QByteArray>
#include <QColor>
#include <QBrush>
#include <iostream>
#include <QMessageBox>

zuluCrypt::zuluCrypt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::zuluCrypt)
{
	openFileUI.setParent(this);
	openFileUI.setWindowFlags(Qt::Window | Qt::Dialog);

	openPartitionUI.setParent(this);
	openPartitionUI.setWindowFlags(Qt::Window | Qt::Dialog);

	NonSystemPartitions.setParent(this);
	NonSystemPartitions.setWindowFlags(Qt::Window | Qt::Dialog);

	luksopenPartitionUI.setParent(this);
	luksopenPartitionUI.setWindowFlags(Qt::Window | Qt::Dialog);

	addKeyUI.setParent(this);
	addKeyUI.setWindowFlags(Qt::Window | Qt::Dialog);

	deleteKeyUI.setParent(this);
	deleteKeyUI.setWindowFlags(Qt::Window | Qt::Dialog);

	createpartitionUI.setParent(this);
	createpartitionUI.setWindowFlags(Qt::Window | Qt::Dialog);

	createFile.setParent(this);
	createFile.setWindowFlags(Qt::Window | Qt::Dialog);

	createkeyFile.setParent(this);
	createkeyFile.setWindowFlags(Qt::Window | Qt::Dialog);

	trayIcon.setParent(this);
	trayIcon.setIcon(QIcon(QString("/usr/share/icons/zuluCrypt.png")));
	connect((QObject *)&trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayIconAction(QSystemTrayIcon::ActivationReason)));

	item_count = 0 ;

	ui->setupUi(this);
	this->setFixedSize(this->size());

	this->setWindowIcon(QIcon(QString("/usr/share/icons/zuluCrypt.png")));

	ui->tableWidget->setColumnWidth(0,230);

	ui->tableWidget->setColumnWidth(1,290);

	ui->tableWidget->setColumnWidth(2,90);

	connect(ui->actionCreatekeyFile,SIGNAL(triggered()),(QObject *)&createkeyFile,SLOT(ShowUI()));

	connect((QObject *)&createFile,SIGNAL(fileCreated(QString)),(QObject *)&createpartitionUI,SLOT(ShowFileUI(QString)));

	connect(ui->actionFileCreate,SIGNAL(triggered()),(QObject *)&createFile,SLOT(showUI()));

	connect((QObject *)&createpartitionUI,SIGNAL(CreateVolume(QString,QString,QString,QString,bool)),this,SLOT(createEncryptedVolume(QString,QString,QString,QString,bool)));

	connect((QObject *)&NonSystemPartitions,SIGNAL(clickedPartition(QString)),(QObject *)&createpartitionUI,SLOT(ShowPartitionUI(QString)));

	connect(this,SIGNAL(showNonSystemPartitions(QStringList)),(QObject *)&NonSystemPartitions,SLOT(ShowNonSystemPartitions(QStringList)));

	connect(this, SIGNAL(luksDeleteKeyUI(QString)),(QObject *)&deleteKeyUI,SLOT(deleteKey(QString))) ;

	connect(this,SIGNAL(luksAddKeyUI(QString)),(QObject *)&addKeyUI,SLOT(partitionEntry(QString))) ;

	connect((QObject *)&openFileUI,SIGNAL(pbOpenClicked(bool,bool,QString,QString,QString)),this,SLOT(openEncryptedVolume(bool,bool,QString,QString,QString))) ;

	connect(ui->actionFileOpen,SIGNAL(triggered()),(QObject *)&openFileUI,SLOT(ShowUI())) ;

	connect(ui->actionPartitionOpen,SIGNAL(triggered()),(QObject *)&openPartitionUI,SLOT(ShowUI()));

	connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(options(QTableWidgetItem*))) ;

	connect((QObject *)&openPartitionUI,SIGNAL(clickedPartition(QString)),(QObject *)&openFileUI,SLOT(clickedPartitionOption(QString)));

	connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutMenuOption())) ;

	connect(ui->actionAddKey,SIGNAL(triggered()), (QObject *)&addKeyUI,SLOT(ShowUI()) ) ;

	connect(ui->actionDeleteKey,SIGNAL(triggered()),(QObject *)&deleteKeyUI,SLOT(ShowUI()) ) ;

	connect((QObject *)&addKeyUI,SIGNAL(clickedpbAdd(QString,bool,QString,bool,QString)), this,SLOT(luksAddKey(QString,bool,QString,bool,QString))) ;

	connect((QObject *)&deleteKeyUI,SIGNAL(pbDeleteClicked(QString,bool,QString)),this, SLOT(luksDeleteKey(QString,bool,QString))) ;

	connect(this,SIGNAL(redoOpen(bool,bool,QString,QString)),(QObject *)&openFileUI,SLOT(ShowUI(bool,bool,QString,QString)));

	connect(ui->actionPartitionCreate,SIGNAL(triggered()),this,SLOT(createEncryptedpartitionUI())) ;

	connect(ui->actionInfo,SIGNAL(triggered()),this,SLOT(info())) ;

	setUpOpenedVolumes() ;

	//trayIcon.show();

	QProcess p ;

	p.start(ZULUCRYPTzuluCrypt);

	p.waitForFinished() ;

	if(  p.exitCode() == 100 ){

		QByteArray T = p.readAllStandardOutput() ;
		UIMessage(QString("WARNING"),QString(T));
	}
	p.close();
}

void zuluCrypt::info()
{
	QProcess exe ;

	QString t = ZULUCRYPTcryptsetup + QString(" --help") ;

	exe.start(t) ;

	exe.waitForFinished() ;

	char *d ;
	char *c = exe.readAllStandardOutput().data() ;

	c = strstr( c , "plain: ") ;

	d = c ;

	while (*++d !=',') { ; }
	*d = '\0' ;

	QString info = QString("cryptographic options used in volume management\n\n") ;

	info = info + QString("type:	PLAIN\n") ;
	info = info + QString("cypher:	") ;
	info = info + QString(c + 7) ;
	info = info + QString("\n") ;
	info = info + QString("keysize:	") ;

	c = strstr( d + 1 , "Key: ") ;
	d = c ;
	while (*++d !=',') { ; }
	*d = '\0' ;

	info = info + QString(c + 5) ;
	info = info + QString("\n") ;

	info = info + QString("hash:	") ;

	c = strstr( d + 1 , "Password") ;
	d = c ;
	while (*++d !='\n') { ; }
	*d = '\0' ;

	info = info + QString(c + 18) ;
	info = info + QString("\n\n") ;

	c = strstr( d + 1 , "LUKS") ;
	d = c ;
	while (*++d !=':') { ; }
	*d = '\0' ;

	info = info + QString("type:	") ;
	info = info + QString(c) ;
	info = info + QString("\n") ;

	c = strstr( d + 1 , "Key") ;
	d = c ;
	while (*++d !=',') { ; }
	*d = '\0' ;

	info = info + QString("keysize:	") ;
	info = info + QString(c + 5) ;
	info = info + QString("\n") ;

	c = strstr( d + 2 , "LUKS") ;
	info = info + QString(c) ;
	info = info + QString("\n") ;

	info = info + QString("maximum passphrase length:	512 characters\n") ;
	info = info + QString("maximum key file size:		512 bytes\n\n") ;

	info = info +QString("device used for creating encrypted files:	/dev/urandom\n") ;
	info = info +QString("device used for creating key files:	/dev/urandom\n") ;

	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.addButton(QMessageBox::Ok);
	m.setWindowTitle(QString("cryptographic info"));

	m.setText(info) ;

	m.exec() ;
	exe.close();
}

void zuluCrypt::trayIconAction(QSystemTrayIcon::ActivationReason e)
{
	//std::cout << QString("dd").toStdString() << std::endl ;
	if ( e == QSystemTrayIcon::Trigger){

		if( this->isHidden() == true)
			this->show();
		else
			this->hide();
	}
}

void zuluCrypt::createEncryptedVolume(QString fileSystem, QString containterType, QString volumePath, QString passphrase,bool passphraseFromFile)
{
	QString N ;

	if (passphraseFromFile == true)
		N = QString("-f") ;
	else
		N = QString("-p") ;

	QString exe = QString(ZULUCRYPTzuluCrypt) + " create \"" + volumePath + "\" " + fileSystem + " " + containterType + " " +  N + " \"" + passphrase + "\"" ;

	QProcess p ;

	p.start(exe);
	p.waitForFinished();

	switch( p.exitCode() ) {
		case 0 : UIMessage(QString("SUCCESS"),QString("volume successfully created"));
			break;
		case 1 : UIMessage(QString("ERROR"),QString("volume path does not exist"));
			break;
		default: UIMessage(QString("ERROR"),(QString("unrecognized error has occured,volume not created")));
	}
	p.close();
}

void zuluCrypt::createEncryptedpartitionUI()
{
	QProcess p ;
	p.start(QString(ZULUCRYPTzuluCrypt) + QString(" partitions"));
	p.waitForFinished() ;

	QStringList l = QString( p.readAllStandardOutput()).split("\n") ;

	p.close();
	emit showNonSystemPartitions( l ) ;
}

void zuluCrypt::luksDeleteKey(QString volumePath,bool passPhraseIsFile, QString pass)
{
	if ( isLuks(volumePath) == false ){

		UIMessage(QString("ERROR"),QString("given path does not point to a luks volume"));
		emit luksDeleteKeyUI(volumePath) ;
		return ;
	}

	QString exe = QString(ZULUCRYPTzuluCrypt) + QString(" removekey ")  + "\"" +volumePath + "\"" ;

	if ( passPhraseIsFile == true ){

		exe = exe + QString(" -f ") ;

	}else{

		exe = exe + QString(" -p ") ;

		for( int i = 0 ; i < pass.size() ; i++){

			if( pass.at(i).toAscii() == '\"'){
				pass.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	exe = exe + QString(" \"") + pass + QString("\"") ;
	QProcess Z ;
	Z.start( exe );
	Z.waitForFinished() ;

	switch( Z.exitCode()  ){
		case 0 :{
				UIMessage(QString("SUCCESS"),QString("key successfully removed\n") + luksEmptySlots(volumePath) + QString(" / 8 slots are now in use"));
				Z.close();
				return ;
			}break ;
		case 1 :	UIMessage(QString("ERROR"),QString("one or more paths has an empty space in them, the back end doesnt like it"));
			break ;
		case 2 :
				UIMessage(QString("ERROR"),QString("there is no key in the volume that match entered key"));
			break ;
		case 4 :	UIMessage(QString("ERROR"),QString("device does not exist"));
			break ;
		case 5 :	UIMessage(QString("ERROR"),QString("key file does not exist"));
			break ;

		default:	UIMessage(QString("ERROR"),QString( "un unexpected error has occured, key not removed "));
	}
	Z.close();
	emit luksDeleteKeyUI(volumePath) ;
}

void zuluCrypt::luksAddKey(QString volumePath, bool keyfile,QString ExistingKey,bool newkeyfile, QString NewKey)
{

	if ( isLuks(volumePath) == false ){

		UIMessage(QString("ERROR"),QString("given path does not point to a luks volume"));
		emit luksAddKeyUI(volumePath) ;
		return ;
	}

	QString existingPassType ;
	QString newPassType ;

	if ( keyfile == true)
		existingPassType = QString(" -f ") ;
	else{
		existingPassType = QString(" -p ") ;

		for( int i = 0 ; i < ExistingKey.size() ; i++){

			if( ExistingKey.at(i).toAscii() == '\"'){
				ExistingKey.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	if ( newkeyfile == true)
		newPassType = QString(" -f ") ;
	else{
		newPassType = QString(" -p ") ;

		for( int i = 0 ; i < NewKey.size() ; i++){

			if( NewKey.at(i).toAscii() == '\"'){
				NewKey.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	QString exe = QString(ZULUCRYPTzuluCrypt) + QString(" addkey ") + "\"" + volumePath + "\"" + existingPassType + "\"" + ExistingKey + "\"" + newPassType + "\"" + NewKey + "\"" ;

	QProcess Z ;

	Z.start(exe);

	Z.waitForFinished() ;

	switch( Z.exitCode() ){
		case 0 :{
				UIMessage(QString("SUCCESS"),QString("key added successfully\n") + luksEmptySlots(volumePath) + QString(" / 8 slots are now in use")) ;
				Z.close();
				return ;
			}break ;
		case 1 :	UIMessage(QString("ERROR"),QString("presented key does not match any key in the volume"));
			break ;
		case 2 :	UIMessage(QString("ERROR"),QString("new passphrases do not match"));
			break ;
		case 3 :	UIMessage(QString("ERROR"),QString("keyfile with the new passphrase does not exist"));
			break ;
		case 4 :	UIMessage(QString("ERROR"),QString("luks volume does not exist"));
			break ;
		default:	UIMessage(QString("ERROR"),QString("un unrecognized error has occured, key not added"));

	}
	Z.close();
	emit luksAddKeyUI(volumePath) ;
}

char zuluCrypt::luksEmptySlots(QString volumePath)
{
	QProcess N ;
	N.start(QString(ZULUCRYPTzuluCrypt) + QString(" emptyslots ") + volumePath);
	N.waitForFinished() ;
	char *s = N.readAllStandardOutput().data() ;
	int i = 0 ;
	for ( int j = 0 ; j < 8 ; j++){
		if( s[j] == '1' )
			i++ ;
	}
	N.close();
	return i + '0' ;
}

bool zuluCrypt::isLuks(QString volumePath)
{
	QProcess N ;
	N.start(QString(ZULUCRYPTzuluCrypt) + QString(" isLuks ") + volumePath);
	N.waitForFinished() ;

	int i = N.exitCode() ;

	N.close();

	if ( i == 0 )
		return true ;
	else
		return false ;
}

void zuluCrypt::aboutMenuOption(void)
{
	//QMessageBox m ;
	//m.setParent(this);
	//m.setWindowFlags(Qt::Window | Qt::Dialog);
	//m.addButton(QMessageBox::Ok);
	//m.setWindowTitle(QString("about zuluCrypt"));

	//m.setText(QString( VERSION_STRING )) ;

	//m.exec() ;

QString a = QString( VERSION_STRING ) + QString("\nThis program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\nSee the GNU General Public License for more details.") ;
	QMessageBox::about(this,QString("About zuluCrypt"), a );
}

void zuluCrypt::setUpOpenedVolumes(void)
{
	QStringList Z =  QDir(QString("/dev/mapper")).entryList().filter("zuluCrypt-") ;

	char *c, *d, *v,*volume,*N ;

	QProcess * p ;

	for ( int i = 0 ; i < Z.size() ; i++){

		QString C = Z.at(i) ;

		C = C.right( C.length() - 10 ) ; //10 is the length of "zuluCrypt-

		p = new QProcess() ;

		p->start(QString(ZULUCRYPTzuluCrypt) + QString(" status ") + C ) ;

		p->waitForReadyRead() ;

		c = p->readAllStandardOutput().data() ;

		p->close();

		p->deleteLater();

		d = strstr(c,"device:") ;

		if ( d == NULL){
			UIMessage(QString("WARNING"),QString("An inconsitency is detected, " + QString(C) + QString(" does not look like a cryptsetp volume")));
			continue ;
		}

		d = d + 9 ;

		if (strncmp(d ,"/dev/loop",9) != 0 ){
			v = d ;
		}else{
			v = strstr(c,"loop:") + 9 ;
		}

		d = v ;

		while ( *++d != '\n') { ; }

		*d = '\0' ;

		N = d = volume = new char[ strlen( v ) + 1 ] ;

		while ( ( *d++ = *v++ ) != '\0') { ; }

		p = new QProcess() ;

		p->start(QString(ZULUCRYPTmount));

		p->waitForReadyRead() ;

		c = p->readAllStandardOutput().data() ;

		v = strrchr(volume,'/') + 1 ;

		if( ( d = strstr(c,v ) ) == NULL ){
			UIMessage(QString("WARNING"),QString("An inconsitency is detected, " + QString(N) + QString(" is opened but not mounted")));
			continue ;
		}

		v = d = d + strlen(v) + 4 ;

		while ( *++v != ' ') { ; }

		*v = '\0' ;

		addItemToTable(QString(volume),QString(d)) ;

		delete [] volume ;

		p->close();

		p->deleteLater();
	}
}

void zuluCrypt::addItemToTable(QString x,QString y)
{
	ui->tableWidget->insertRow(item_count);
	ui->tableWidget->setItem(item_count,0,new QTableWidgetItem(x)) ;
	ui->tableWidget->setItem(item_count,1,new QTableWidgetItem(y)) ;
	if ( isLuks( ui->tableWidget->item(item_count,0)->text()) == true )
		ui->tableWidget->setItem(item_count,2,new QTableWidgetItem(QString("luks"))) ;
	else
		ui->tableWidget->setItem(item_count,2,new QTableWidgetItem(QString("plain"))) ;

	ui->tableWidget->item(item_count,0)->setTextAlignment(Qt::AlignCenter);
	ui->tableWidget->item(item_count,1)->setTextAlignment(Qt::AlignCenter);
	ui->tableWidget->item(item_count,2)->setTextAlignment(Qt::AlignCenter);

	item_count++ ;	
}

void zuluCrypt::removeRowFromTable( int x )
{
	ui->tableWidget->removeRow( x ) ;
	item_count-- ;
}

void zuluCrypt::volume_property()
{
	QProcess p ;

	QString path = item->tableWidget()->item(item->row(),0)->text() ;

	QString z = QString(ZULUCRYPTzuluCrypt) + QString(" status ") +  path ;

	p.start( z ) ;
	p.waitForFinished() ;

	QByteArray t("\n") ;

	QByteArray r = p.readAllStandardOutput() ;

	int start = r.length() - r.indexOf(t) - 2 ;

	p.close();

	QMessageBox m ;
	m.setWindowTitle(QString("volume properties"));
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	if ( isLuks(path) == true)
		m.setText( QString( r.right(start) )   + QString("  occupied key slots: ") + luksEmptySlots(path) + QString(" / 8")) ;
	else
		m.setText( QString( r.right(start) ) ) ;

	m.addButton(QMessageBox::Ok);
	m.exec() ;
}

void zuluCrypt::options(QTableWidgetItem* t)
{
	item = t ;

	QMenu m ;

	connect(m.addAction("close"),SIGNAL(triggered()),this,SLOT(close())) ;

	m.addSeparator() ;

	connect(m.addAction("properties"),SIGNAL(triggered()),this,SLOT(volume_property())) ;

	QProcess Z ;

	Z.start( QString(ZULUCRYPTzuluCrypt) + QString(" isLuks ") + ui->tableWidget->item(item->row(),0)->text());

	Z.waitForFinished() ;

	if( Z.exitCode() == 0 ){
		m.addSeparator() ;
		connect(m.addAction("add key"),SIGNAL(triggered()),this,SLOT(luksAddKeyContextMenu())) ;
		connect(m.addAction("remove key"),SIGNAL(triggered()),this,SLOT(luksDeleteKeyContextMenu())) ;
	}
	Z.close();
	m.exec(QCursor::pos()) ;
}

void zuluCrypt::luksAddKeyContextMenu(void)
{
	emit luksAddKeyUI(ui->tableWidget->item(item->row(),0)->text() ) ;
}

void zuluCrypt::luksDeleteKeyContextMenu(void)
{
	emit luksDeleteKeyUI(ui->tableWidget->item(item->row(),0)->text()) ;
}

void zuluCrypt::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.exec() ;
}

void zuluCrypt::close(void)
{
	QProcess p ;

	p.start(QString(ZULUCRYPTzuluCrypt) + QString(" close ") + ui->tableWidget->item(item->row(),1)->text() ) ;

	p.waitForFinished() ;

	switch ( p.exitCode() ) {
	case 0 :	removeRowFromTable(item->row()) ;
		break ;
	case 1 :	UIMessage(QString("ERROR"),QString("close failed, encrypted volume with that name does not exist")) ;
		break ;

	case 2 :	UIMessage(QString("ERROR"),QString("close failed, the mount point and/or one or more files are in use"));
		break ;
	case 3 :	UIMessage(QString("ERROR"),QString("close failed, path given does not point to a file"));
		break ;
	default :	UIMessage(QString("ERROR"),QString("an unknown error has occured, volume not closed"));
	}
	p.close();
}

//hopefully secure delete,write random data to a file and then delete it
void zuluCrypt::deleteFile( QFile *f)
{
	if ( f == NULL)
		return ;

	QFile in(QString("/dev/urandom")) ;

	in.open(QIODevice::ReadOnly) ;

	f->seek(0) ;

	char z ;

	for(int i = 0 ; i < f->size() ; i++){
		in.getChar(&z) ;
		f->putChar(z) ;
	}
	in.close();
	f->close();
	f->remove() ;
}

void zuluCrypt::openEncryptedVolume(bool boolOpenReadOnly,bool boolKeyFromFile,QString volumePath, QString mountPointPath,QString passPhraseField)
{
	QString exe ;
	QString mode ;
	QString passtype ;

	if ( boolOpenReadOnly == true )
		mode = "ro" ;
	else
		mode = "rw" ;

	QProcess process ;

	if ( boolKeyFromFile == true ){

		passtype = " -f " ;

	}else{
		passtype = " -p " ;

		for( int i = 0 ; i < passPhraseField.size() ; i++){

			if( passPhraseField.at(i).toAscii() == '\"'){
				passPhraseField.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	process.start(exe) ;

	process.waitForFinished() ;

	exe = QString(ZULUCRYPTzuluCrypt) + " open \"" + volumePath + "\" \"" + mountPointPath + "\" " + mode + " " + passtype + "\"" + passPhraseField +"\"";

	process.start(exe) ;

	process.waitForFinished() ;
	int T = process.exitCode() ;

	process.close();

	switch ( T ){
	case 0 :{
		/*
		  There are possible names zuluCrypt-cli will use for mount point and predicting it before hand may
		  cause unnecessary code bloat. If the opening succeed, just go read the output of "mount"
		  and use whatever you will find.
		  */
		char * c = 0 ;

		char *d = 0 ;
		int k ;
		QString N ;
		QProcess Z ;
		Z.start(QString(ZULUCRYPTmount));

		Z.waitForFinished() ;

		c = Z.readAllStandardOutput().data() ;

		N = "/dev/mapper/zuluCrypt-" + volumePath.split("/").last() ;

		d = N.toAscii().data() ;

		k = strlen( d ) ;

		d = c = strstr( c , d )  + k + 4 ;

		while (*++d != ' ') { ; }

		*d = '\0' ;

		addItemToTable(volumePath,QString( c ));

		Z.close();

		return ;
		}break ;

	case 1 : UIMessage(QString("ERROR"),QString("No free loop device to use.")) ;
		return ;
		break ;

	case 2 : UIMessage(QString("ERROR"),QString("there seem to be an open volume accociated with given path."));
		break ;

	case 3 : UIMessage(QString("ERROR"),QString("no file or device on a given address.")) ;
		break ;

	case 4 :
		UIMessage(QString("ERROR"),QString("wrong passphrase."));
		break ;

	case 5 : UIMessage(QString("ERROR"),QString("mount point address is already taken by a file or folder")) ;
		break ;
	case 6 : UIMessage(QString("ERROR"),QString("passphrase file does not exist"));
		break ;
	case 9 : UIMessage(QString("ERROR"),QString("\",\" (comma) is not a valid mount point"));
		break ;
	default :UIMessage(QString("ERROR"),QString("un unknown error has occured, volume not opened"));
	}

	emit redoOpen(boolOpenReadOnly,boolKeyFromFile,volumePath, mountPointPath);
}

zuluCrypt::~zuluCrypt()
{
	delete ui;
}
