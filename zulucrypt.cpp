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
#include <QFontDialog>
#include <QMetaType>

Q_DECLARE_METATYPE(Qt::Orientation) ;

zuluCrypt::zuluCrypt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::zuluCrypt)
{
	item_count = 0 ;

	qRegisterMetaType<Qt::Orientation>("Qt::Orientation") ;

	openFileUI = new password_Dialog(this) ;
	openFileUI->setWindowFlags(Qt::Window | Qt::Dialog);

	openPartitionUI = new openpartition(this);
	openPartitionUI->setWindowFlags(Qt::Window | Qt::Dialog);

	NonSystemPartitions = new openpartition(this);
	NonSystemPartitions->setWindowFlags(Qt::Window | Qt::Dialog);

	luksopenPartitionUI = new openpartition(this);
	luksopenPartitionUI->setWindowFlags(Qt::Window | Qt::Dialog);

	addKeyUI = new luksaddkeyUI(this);
	addKeyUI->setWindowFlags(Qt::Window | Qt::Dialog);

	deleteKeyUI = new luksdeletekey(this);
	deleteKeyUI->setWindowFlags(Qt::Window | Qt::Dialog);

	createpartitionUI = new createpartition(this);
	createpartitionUI->setWindowFlags(Qt::Window | Qt::Dialog);

	createFile = new createfile(this);
	createFile->setWindowFlags(Qt::Window | Qt::Dialog);

	createkeyFile = new createkeyfile(this);
	createkeyFile->setWindowFlags(Qt::Window | Qt::Dialog);

	rng = new rngselector(this);
	rng->setWindowFlags(Qt::Window | Qt::Dialog);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(QString("/usr/share/icons/zuluCrypt.png")));

	ui->setupUi(this);
	this->setFixedSize(this->size());

	this->setWindowIcon(QIcon(QString("/usr/share/icons/zuluCrypt.png")));

	ui->tableWidget->setColumnWidth(0,230);

	ui->tableWidget->setColumnWidth(1,290);

	ui->tableWidget->setColumnWidth(2,90);

	connect(ui->actionCreatekeyFile,SIGNAL(triggered()),createkeyFile,SLOT(ShowUI()));

	connect(createFile,SIGNAL(fileCreated(QString)),createpartitionUI,SLOT(ShowFileUI(QString)));

	connect(ui->actionFileCreate,SIGNAL(triggered()),createFile,SLOT(showUI()));

	connect(NonSystemPartitions,SIGNAL(clickedPartition(QString)),createpartitionUI,SLOT(ShowPartitionUI(QString)));

	connect(this,SIGNAL(showNonSystemPartitions()),NonSystemPartitions,SLOT(ShowNonSystemPartitions()));

	connect(ui->actionFileOpen,SIGNAL(triggered()),openFileUI,SLOT(ShowUI())) ;

	connect(ui->actionPartitionOpen,SIGNAL(triggered()),openPartitionUI,SLOT(ShowUI()));

	connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(options(QTableWidgetItem*))) ;

	connect(openPartitionUI,SIGNAL(clickedPartition(QString)),openFileUI,SLOT(clickedPartitionOption(QString)));

	connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutMenuOption())) ;

	connect(ui->actionAddKey,SIGNAL(triggered()), addKeyUI,SLOT(ShowUI()) ) ;

	connect(ui->actionDeleteKey,SIGNAL(triggered()),deleteKeyUI,SLOT(ShowUI()) ) ;

	connect(this,SIGNAL(redoOpen(bool,bool,QString,QString)),openFileUI,SLOT(ShowUI(bool,bool,QString,QString)));

	connect(ui->actionPartitionCreate,SIGNAL(triggered()),this,SLOT(createEncryptedpartitionUI())) ;

	connect(ui->actionInfo,SIGNAL(triggered()),this,SLOT(info())) ;

	connect(ui->actionFonts,SIGNAL(triggered()),this,SLOT(fonts())) ;

	connect(ui->menuFavorites,SIGNAL(aboutToShow()),this,SLOT(readFavorites())) ;

	connect(ui->menuFavorites,SIGNAL(aboutToHide()),this,SLOT(favAboutToHide())) ;

	connect(ui->actionTray_icon,SIGNAL(triggered()),this,SLOT(trayProperty())) ;

	connect(this,SIGNAL(favClickedVolume(QString,QString)),openFileUI,SLOT(ShowUI(QString,QString))) ;

	connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));

	connect(ui->menuFavorites,SIGNAL(triggered(QAction*)),this,SLOT(favClicked(QAction*))) ;

	connect(ui->actionSelect_random_number_generator,SIGNAL(triggered()),rng,SLOT(ShowUI())) ;

	connect(openFileUI,SIGNAL(addItemToTable(QString,QString)),this,SLOT(addItemToTable(QString,QString))) ;
	setUpOpenedVolumes() ;

	QProcess p ;

	p.start(ZULUCRYPTzuluCrypt);

	p.waitForFinished() ;

	if(  p.exitCode() == 100 ){

		QByteArray T = p.readAllStandardOutput() ;
		UIMessage(QString("WARNING"),QString(T));
	}
	p.close();

	QString home = QDir::homePath() + QString("/.zuluCrypt/") ;

	QDir d(home) ;

	if(d.exists() == false)
		d.mkdir(home) ;

	QFile f(QDir::homePath() + QString("/.zuluCrypt/tray")) ;

	if(f.exists() == false){

		f.open(QIODevice::WriteOnly | QIODevice::Truncate) ;
		f.write("1") ;
		f.close();
	}

	f.open(QIODevice::ReadOnly) ;

	QByteArray t = f.readAll() ;

	f.close() ;

	char *c = t.data() ;

	ui->actionTray_icon->setCheckable(true);

	if( c[0] == '1'){
		ui->actionTray_icon->setChecked(true);
		trayIcon->show();
	}else{
		ui->actionTray_icon->setChecked(false);
		trayIcon->hide();
	}

	QFile g(QDir::homePath() + QString("/.zuluCrypt/rng")) ;

	if(g.exists() == false){

		g.open(QIODevice::WriteOnly | QIODevice::Truncate) ;
		g.write("/dev/urandom") ;
		g.close();
	}

	QFile z(QDir::homePath() + QString("/.zuluCrypt/font")) ;

	if(z.exists() == false){

		z.open(QIODevice::WriteOnly | QIODevice::Truncate) ;

		QString s = QString("Sans Serif\n9\nnormal\nnormal") ;

		z.write( s.toAscii() ) ;

		z.close();
	}

	QFile x(QDir::homePath() + QString("/.zuluCrypt/font"));

	x.open(QIODevice::ReadOnly) ;

	QStringList xs = QString( x.readAll() ).split("\n") ;

	x.close();

	QFont F ;

	F.setFamily(xs.at(0));
	F.setPointSize(xs.at(1).toInt());

	if(xs.at(2) == QString("normal"))
		F.setStyle(QFont::StyleNormal);
	else if(xs.at(2) == QString("italic"))
		F.setStyle(QFont::StyleItalic);
	else
		F.setStyle(QFont::StyleOblique);

	if(xs.at(3) == QString("normal"))
		F.setWeight(QFont::Normal);
	else
		F.setWeight(QFont::Bold);

	setUserFont(F);

	setUserFont(F);
}

void zuluCrypt::trayClicked(QSystemTrayIcon::ActivationReason e)
{
	std::cout << "ds" << std::endl ;
	if( e == QSystemTrayIcon::Trigger){

		if(this->isVisible() == true)
			this->hide();
		else
			this->show();
	}
}

void zuluCrypt::trayProperty()
{
	QFile f(QDir::homePath() + QString("/.zuluCrypt/tray")) ;

	//if(in.exists() == true)
	//	std::cout << "blah" << std::endl ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray t = f.readAll() ;

	f.close();

	f.open(QIODevice::WriteOnly | QIODevice::Truncate) ;

	char *c = t.data() ;

	char data[2] ;

	data[1] = '\0' ;

	//std::cout << t.data() << std::endl ;

	if(c[0] == '1'){
		data[0] = '0' ;
		f.write(data) ;
		trayIcon->hide();
	}else{
		data[0] = '1' ;
		f.write(data) ;
		trayIcon->show();
	}

	f.close();
}

void zuluCrypt::fonts()
{
	bool ok ;
	QFont Font = QFontDialog::getFont(&ok,this->font(),this) ;

	if( ok == true ){

		QByteArray ba ;

		int k = Font.pointSize() ;

		do{
			ba.push_front( k % 10 + '0' ) ;
			k = k / 10 ;

		}while( k != 0 ) ;

		setUserFont(Font);

		QString s = Font.family()+ QString("\n");

		s = s + QString( ba )  + QString("\n") ;

		if(Font.style() == QFont::StyleNormal)

			s = s + QString("normal\n") ;

		else if(Font.style() == QFont::StyleItalic)

			s = s + QString("italic\n") ;
		else

			s = s + QString("oblique\n") ;

		if(Font.weight() == QFont::Normal)

			s = s + QString("normal\n") ;
		else
			s = s + QString("bold") ;

		QFile f(QDir::homePath() + QString("/.zuluCrypt/font")) ;

		f.open(QIODevice::WriteOnly | QIODevice::Truncate ) ;
		f.write(s.toAscii()) ;
		f.close();
	}
}

void zuluCrypt::setUserFont(QFont Font)
{
	this->setFont(Font);

	ui->tableWidget->horizontalHeaderItem(0)->setFont(Font);
	ui->tableWidget->horizontalHeaderItem(1)->setFont(Font);
	ui->tableWidget->horizontalHeaderItem(2)->setFont(Font);

	ui->actionAbout->setFont(this->font());
	ui->actionAddKey->setFont(this->font());
	ui->actionCreatekeyFile->setFont(this->font());
	ui->actionDeleteKey->setFont(this->font());
	ui->actionFavorite_volumes->setFont(this->font());
	ui->actionFileCreate->setFont(this->font());
	ui->actionFileOpen->setFont(this->font());
	ui->actionFonts->setFont(this->font());
	ui->actionInfo->setFont(this->font());
	ui->actionManage_favorites->setFont(this->font());
	ui->actionPartitionCreate->setFont(this->font());
	ui->actionPartitionOpen->setFont(this->font());
	ui->actionSelect_random_number_generator->setFont(this->font());
	ui->actionTray_icon->setFont(this->font());
	ui->menuFavorites->setFont(this->font());

	openFileUI->setFont(Font);

	openPartitionUI->setFont(Font);

	NonSystemPartitions->setFont(Font);

	luksopenPartitionUI->setFont(Font);

	addKeyUI->setFont(Font);

	deleteKeyUI->setFont(Font);

	createpartitionUI->setFont(Font);

	createFile->setFont(Font);

	createkeyFile->setFont(Font);
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
	//info = info + QString("maximum key file size:		512 bytes\n\n") ;

	info = info + QString("key files are generated with:	32 characters(256bits)\n\n") ;

	info = info +QString("device used for creating encrypted files:	( user specified )\n") ;
	info = info +QString("device used for creating key files:	( user specified )\n") ;

	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.addButton(QMessageBox::Ok);
	m.setWindowTitle(QString("cryptographic info"));
	m.setFont(this->font());
	m.setText(info) ;

	m.exec() ;
	exe.close();
}

void zuluCrypt::createEncryptedpartitionUI()
{
	emit showNonSystemPartitions( ) ;
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
	N.start(QString(ZULUCRYPTzuluCrypt) + QString(" isLuks ") + QString("\"") + volumePath + QString("\"") );
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
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.addButton(QMessageBox::Ok);
	m.setWindowTitle(QString("about zuluCrypt"));
	m.setFont(this->font());
	QString a = QString( VERSION_STRING ) + QString("\nThis program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\nSee the GNU General Public License for more details.") ;
	m.setText( a ) ;
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
	mp = new QMessageBox(this) ;
	mp->setWindowTitle(QString("volume properties"));
	mp->setParent(this);
	mp->setWindowFlags(Qt::Window | Qt::Dialog);
	mp->addButton(QMessageBox::Ok);
	mp->setFont(this->font());

	QString path = item->tableWidget()->item(item->row(),0)->text() ;

	vpt = new volumePropertiesThread(path,&volumeProperty) ;

	connect(vpt,SIGNAL(finished()),this,SLOT(volumePropertyThreadFinished())) ;

	vpt->start();
}

void zuluCrypt::volumePropertyThreadFinished()
{
	delete vpt ;
	mp->setText(volumeProperty);
	mp->exec() ;
	delete mp ;
}

void zuluCrypt::favAboutToHide()
{
	ui->menuFavorites->clear();
}

void zuluCrypt::favClicked(QAction *e)
{
	QMenu *m = (QMenu *) e->parentWidget() ;

	if( e->text() == QString("open") ){

		QStringList l =  m->title().split("\t") ;

		emit favClickedVolume(l.at(0),l.at(1));

	}else if( e->text() == QString("remove from favorite") ){

		QFile f(QDir::homePath() + QString("/.zuluCrypt/favorites")) ;

		f.open(QIODevice::ReadOnly) ;

		QByteArray b = f.readAll() ;

		f.close();

		QByteArray c = b.remove(b.indexOf(m->title()),m->title().length() + 1) ;

		f.open(QIODevice::WriteOnly | QIODevice::Truncate) ;

		f.write( c ) ;

		f.close() ;
	}
}

void zuluCrypt::readFavorites()
{
	QFile f(QDir::homePath() + QString("/.zuluCrypt/favorites")) ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray b = f.readAll() ;

	f.close();

	QStringList l = QString( b ).split("\n") ;

	for(int i = 0 ; i < l.size() - 1 ; i++){

		QMenu *m = new QMenu(l.at(i)) ;
		m->setFont(this->font());
		m->addAction("open")->setParent(m);

		m->addAction("remove from favorite")->setParent(m); ;

		ui->menuFavorites->addMenu(m) ;

		menulist.append(m);
	}
}

void zuluCrypt::addToFavorite()
{
	QString volume_path = ui->tableWidget->item(item->row(),0)->text() ;

	QString mount_point_path = ui->tableWidget->item(item->row(),1)->text();

	int i = mount_point_path.lastIndexOf("/") ;

	mount_point_path = mount_point_path.left( i ) ;

	QString fav = volume_path + QString("\t") + mount_point_path + QString("\n") ;

	QFile f(QDir::homePath() + QString("/.zuluCrypt/favorites")) ;

	f.open(QIODevice::WriteOnly | QIODevice::Append ) ;

	f.write(fav.toAscii()) ;

	f.close();
}

void zuluCrypt::options(QTableWidgetItem* t)
{
	item = t ;

	QMenu m ;
	m.setFont(this->font());
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
	
	m.addSeparator() ;
	
	QString x = ui->tableWidget->item(item->row(),0)->text() ;
	QString y = ui->tableWidget->item(item->row(),1)->text() ;

	QString volume_path = ui->tableWidget->item(item->row(),0)->text() ;
	QString mount_point_path = ui->tableWidget->item(item->row(),1)->text();

	int i = mount_point_path.lastIndexOf("/") ;

	mount_point_path = mount_point_path.left( i ) ;

	QString fav = volume_path + QString("\t") + mount_point_path ;

	QFile f(QDir::homePath() + QString("/.zuluCrypt/favorites")) ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray data = f.readAll() ;

	QAction a(QString("add to favorite"),(QObject *)&m) ;

	if( strstr( data.data() , fav.toAscii().data() ) == NULL )
		a.setEnabled(true);
	else
		a.setEnabled(false);

	a.connect((QObject *)&a,SIGNAL(triggered()),this,SLOT(addToFavorite())) ;

	m.addAction(&a);
	
	Z.close();
	m.setFont(this->font());
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

zuluCrypt::~zuluCrypt()
{	
	for( int i = 0 ; i < menulist.size() ; i++){

		delete menulist.at(i) ;
	}

	menulist.clear();

	delete openFileUI ;

	delete openPartitionUI ;

	delete NonSystemPartitions ;

	delete luksopenPartitionUI ;

	delete createpartitionUI ;

	delete addKeyUI ;

	delete deleteKeyUI ;

	delete createFile ;

	delete createkeyFile ;

	delete ui;
}
