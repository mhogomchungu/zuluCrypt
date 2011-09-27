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

#include <stdio.h>

zuluCrypt::zuluCrypt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::zuluCrypt)
{
	zuluCryptExe = "/usr/bin/zuluCrypt-cli" ;

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


	item_count = 0 ;

	ui->setupUi(this);
	this->setFixedSize(this->size());

	this->setWindowIcon(QIcon(QString("/usr/share/icons/zuluCrypt.png")));

	ui->tableWidget->setColumnWidth(0,230);

	ui->tableWidget->setColumnWidth(1,290);

	ui->tableWidget->setColumnWidth(2,90);

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

	setUpOpenedVolumes() ;
}

void zuluCrypt::createEncryptedVolume(QString fileSystem, QString containterType, QString volumePath, QString passphrase,bool passphraseFromFile)
{
	QString N ;

	if (passphraseFromFile == true)
		N = QString("-f") ;
	else
		N = QString("-p") ;

	QString exe = zuluCryptExe + " create \"" + volumePath + "\" " + fileSystem + " " + containterType + " " +  N + " \"" + passphrase + "\"" ;

	std::cout << exe.toStdString() << std::endl ;

	QProcess p ;

	p.start(exe);
	p.waitForFinished();

	switch( p.exitCode() ) {
		case 0 : UIMessage(QString("SUCCESS: volume successfully created"));
			break;
		case 1 : UIMessage(QString("ERROR: volume path does not exist"));
			break;
		default: UIMessage((QString("ERROR: unrecognized error has occured,volume not created")));
	}
}

void zuluCrypt::createEncryptedpartitionUI()
{
	QProcess p ;
	p.start(zuluCryptExe + QString(" partitions"));
	p.waitForFinished() ;

	QStringList l = QString( p.readAllStandardOutput()).split("\n") ;

	emit showNonSystemPartitions( l ) ;
}

void zuluCrypt::luksDeleteKey(QString volumePath,bool passPhraseIsFile, QString p)
{
	QString exe = zuluCryptExe + QString(" removekey ")  + "\"" +volumePath + "\"" ;

	if ( passPhraseIsFile == true )
		exe = exe + QString(" -f ") ;
	else
		exe = exe + QString(" -p ") ;

	exe = exe + "\"" + p + "\"" ;

	QProcess Z ;
	Z.start( exe );
	Z.waitForFinished() ;

	switch( Z.exitCode() ){
		case 0 :	UIMessage(QString("SUCCESS: key successfully removed\n") + luksEmptySlots(volumePath) + QString(" / 8 slots are now in use"));
			break ;
		case 1 :	UIMessage(QString("ERROR: one or more paths has an empty space in them, the back end doesnt like it"));
			break ;
		case 2 :{
				emit luksDeleteKeyUI(volumePath) ;
				UIMessage(QString("ERROR: there is no key in the volume that match entered key"));
			}break ;
		case 4 :	UIMessage(QString("ERROR: device does not exist"));
			break ;
		case 5 :	UIMessage(QString("ERROR: key file does not exist"));
			break ;
		case 7 : UIMessage(QString("ERROR: creating volumes on system partitions is not supported"));
			break ;
		default:	UIMessage(QString("ERROR: an unknown error has occured, key not deleted"));
	}
}

void zuluCrypt::luksAddKey(QString volumePath, bool keyfile,QString ExistingKey,bool newkeyfile, QString NewKey)
{
	QString existingPassType ;
	QString newPassType ;

	if ( keyfile == true)
		existingPassType = QString(" -f ") ;
	else
		existingPassType = QString(" -p ") ;

	if ( newkeyfile == true)
		newPassType = QString(" -f ") ;
	else
		newPassType = QString(" -p ") ;

	QString exe = zuluCryptExe + QString(" addkey ") + "\"" + volumePath + "\"" + existingPassType + "\"" + ExistingKey + "\"" + newPassType + "\"" + NewKey + "\"" ;

	QProcess Z ;

	Z.start(exe);

	Z.waitForFinished() ;

	switch( Z.exitCode() ){
		case 0 :	UIMessage(QString("SUCCESS: key added successfully\n") + luksEmptySlots(volumePath) + QString(" / 8 slots are now in use")) ;
			break ;
		case 1 :	UIMessage(QString("ERROR: one or more paths has an empty space in them, the back end doesnt like it"));
			break ;
		case 2 :{
				UIMessage(QString("ERROR: presented key does not match any key in the volume"));
				emit luksAddKeyUI(volumePath) ;
			}break ;
		case 3 :	UIMessage(QString("ERROR: keyfile with the new passphrase does not exist"));
			break ;
		case 4 :	UIMessage(QString("ERROR: luks volume does not exist"));
			break ;
		default:	UIMessage(QString("ERROR: un unrecognized error has occured, key not added"));

	}
}

char zuluCrypt::luksEmptySlots(QString volumePath)
{
	QProcess N ;
	N.start(QString(zuluCryptExe + QString(" emptyslots ")) + volumePath);
	N.waitForFinished() ;
	char *s = N.readAllStandardOutput().data() ;
	int i = 0 ;
	for ( int j = 0 ; j < 8 ; j++){
		if( s[j] == '1' )
			i++ ;
	}
	return i + '0' ;
}

bool zuluCrypt::isLuks(QString volumePath)
{
	QProcess N ;
	N.start(QString(zuluCryptExe + QString(" isLuks ")) + volumePath);
	N.waitForFinished() ;

	if ( N.exitCode() == 0 )
		return true ;
	else
		return false ;
}

void zuluCrypt::aboutMenuOption(void)
{
	QMessageBox m ;

	m.addButton(QMessageBox::Ok);

	m.setText(QString( VERSION_STRING )) ;

	m.exec() ;
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

		p->start(zuluCryptExe + " status " + C ) ;

		p->waitForReadyRead() ;

		c = p->readAllStandardOutput().data() ;

		p->close();

		p->deleteLater();

		d = strstr(c,"device:") ;

		if ( d == NULL){
			UIMessage(QString("WARNING: An inconsitency is detected, " + QString(C) + QString(" does not look like a cryptsetp volume")));
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

		p->start("mount");

		p->waitForReadyRead() ;

		c = p->readAllStandardOutput().data() ;

		v = strrchr(volume,'/') + 1 ;

		if( ( d = strstr(c,v ) ) == NULL ){
			UIMessage(QString("WARNING: An inconsitency is detected, " + QString(N) + QString(" is opened but not mounted")));
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

	QString z = zuluCryptExe + " status " +  path ;

	p.start( z ) ;
	p.waitForFinished() ;

	QString M = p.readAllStandardOutput() ;

	char *c = M.toAscii().data() ;

	int i = 0 ;

	while ( *c++ != '\n') { i++ ; }

	QMessageBox m ;

	if ( isLuks(path) == true)
		m.setText( M.right( M.size() - i) + QString("  occupied key slots: ") + luksEmptySlots(path) + QString(" / 8")) ;
	else
		m.setText( M.right( M.size() - i) ) ;

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

	Z.start( zuluCryptExe + QString(" isLuks ") + ui->tableWidget->item(item->row(),0)->text());

	Z.waitForFinished() ;

	if( Z.exitCode() == 0 ){
		m.addSeparator() ;
		connect(m.addAction("add key"),SIGNAL(triggered()),this,SLOT(luksAddKeyContextMenu())) ;
		connect(m.addAction("remove key"),SIGNAL(triggered()),this,SLOT(luksDeleteKeyContextMenu())) ;
	}
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

void zuluCrypt::UIMessage(QString message)
{
	QMessageBox m ;
	m.setText(message);
	m.addButton(QMessageBox::Ok);
	m.exec() ;
}

void zuluCrypt::close(void)
{
	QProcess p ;

	p.start(zuluCryptExe + " close " + ui->tableWidget->item(item->row(),0)->text() ) ;
	p.waitForFinished() ;

	switch ( p.exitCode() ) {
	case 0 :	removeRowFromTable(item->row()) ;
		break ;
	case 1 :	UIMessage(QString("ERROR: close failed, encrypted volume with that name does not exist")) ;
		break ;

	case 2 :	UIMessage(QString("ERROR: close failed, the mount point and/or one or more files are in use"));
		break ;
	default :	UIMessage(QString("ERROR: an unknown error has occured, volume not closed"));
	}
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

	if ( boolKeyFromFile == true )
		passtype = " -f " ;
	else
		passtype = " -p " ;

	exe = zuluCryptExe + " open \"" + volumePath + "\" \"" + mountPointPath + "\" " + mode + " " + passtype + "\"" + passPhraseField +"\"";

	QProcess process ;
	process.start(exe) ;
	process.waitForFinished() ;

	switch ( process.exitCode() ){
	case 0 : {
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
		Z.start(QString("mount"));

		Z.waitForFinished() ;

		c = Z.readAllStandardOutput().data() ;

		N = "/dev/mapper/zuluCrypt-" + volumePath.split("/").last() ;

		d = N.toAscii().data() ;

		k = strlen( d ) ;

		d = c = strstr( c , d )  + k + 4 ;

		while (*++d != ' ') { ; }

		*d = '\0' ;

		addItemToTable(volumePath,QString( c ));

		} break ;

	case 1 : UIMessage(QString("ERROR: No free loop device to use.")) ;
		break ;

	case 2 : UIMessage(QString("ERROR: There seem to be an open volume accociated with given path."));
		break ;

	case 3 : UIMessage(QString("ERROR: No file or device on a given address.")) ;
		break ;

	case 4 :{
			UIMessage(QString("ERROR: Wrong passphrase."));
			emit redoOpen(boolOpenReadOnly,boolKeyFromFile,volumePath, mountPointPath);
		}break ;

	case 5 : UIMessage(QString("ERROR: mount point address is already taken by a file or folder")) ;
		break ;
	case 6 : UIMessage(QString("ERROR: passphrase file does not exist"));

	case 9 : UIMessage(QString("ERROR: \",\" (comma) is not a valid mount point"));
		break ;
	default :UIMessage(QString("ERROR: un unknown error has occured, volume not opened"));
	}
}

zuluCrypt::~zuluCrypt()
{
	delete ui;
}
