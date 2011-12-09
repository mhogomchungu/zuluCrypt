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
#include <QDebug>

Q_DECLARE_METATYPE(Qt::Orientation) ;
Q_DECLARE_METATYPE(QItemSelection) ;

zuluCrypt::zuluCrypt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::zuluCrypt)
{
	item_count = 0 ;

	selectedRow = -1 ;

	qRegisterMetaType<Qt::Orientation>("Qt::Orientation") ;

	qRegisterMetaType<QItemSelection>("QItemSelection") ;

	setupUIElements();

	setupConnections();

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

	QByteArray c = f.readAll() ;

	f.close() ;

	ui->actionTray_icon->setCheckable(true);

	if( c.at(0) == '1'){
		ui->actionTray_icon->setChecked(true);
		trayIcon->show();
	}else{
		ui->actionTray_icon->setChecked(false);
		trayIcon->hide();
	}

	QString fontPath = QDir::homePath() + QString("/.zuluCrypt/font") ;

	QFile z( fontPath ) ;

	if(z.exists() == false){

		z.open(QIODevice::WriteOnly | QIODevice::Truncate) ;

		QString s = QString("Sans Serif\n9\nnormal\nnormal\n") ;

		z.write( s.toAscii() ) ;

		z.close();
	}

	QFile x( fontPath );

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

	crh = new ClickedRowHighlight() ;

	vpt = new volumePropertiesThread() ;

	connect(vpt,
		SIGNAL(finished()),
		this,
		SLOT(volumePropertyThreadFinished())) ;

	sov = new startupupdateopenedvolumes(this);

	connect(sov,
		SIGNAL(addItemToTable(QString,QString)),
		this,
		SLOT(addItemToTable(QString,QString))) ;

	connect(sov,
		SIGNAL(finished()),
		this,
		SLOT(sovfinished())) ;

	connect(sov,
		SIGNAL(UIMessage(QString,QString)),
		this,
		SLOT(UIMessage(QString,QString))) ;

	sov->start();
}

void zuluCrypt::closeAllVolumes()
{
	t = new closeAllVolumesThread(ui->tableWidget) ;

	connect(t,SIGNAL(close(QTableWidgetItem *,int)),this,SLOT(closeAll(QTableWidgetItem *,int))) ;

	connect(t,SIGNAL(finished()),this,SLOT(deleteThread())) ;

	t->start();
}

void zuluCrypt::deleteThread()
{
	delete t ;
}

void zuluCrypt::closeAll(QTableWidgetItem * i,int st)
{
	if( st == 0 )
		removeRowFromTable(i->row());
	else{
		QString msg = tr("Could not close \"") + \
			      ui->tableWidget->item(i->row(),1)->text() + \
				tr("\" because the mount point and/or one or more files are in use") ;
		UIMessage(QString("ERROR!"),msg);
	}
}

void zuluCrypt::minimize()
{
	zuluCrypt::setWindowState(Qt::WindowMinimized) ;
}

void zuluCrypt::minimizeToTray()
{
	if( ui->actionTray_icon->isChecked() == true ){
		this->hide();
	}else{
		ui->actionTray_icon->setChecked(true);
		trayProperty() ;
		this->hide();
	}
}

QString zuluCrypt::mtab(QString entry)
{
	QFile mt(QString("/etc/mtab")) ;

	QByteArray sc = "#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ;
	
	for( int n = 0 ; n < sc.size() ; n++){
		
		entry.replace(QChar(sc.at(n)),QChar('_')) ;
	}	
	
	mt.open(QIODevice::ReadOnly) ;

	QByteArray data = mt.readAll() ;

	mt.close();

	int i = data.indexOf(entry) ;

	if( i == -1 )
		return QString("") ;

	while(data.at(i++) != ' ') { ; }

	int j = i ;

	while(data.at(i++) != ' ') { ; }

	QByteArray mount_point = data.mid(j, i - j - 1) ;

	mount_point.replace("\\040"," ") ;

	return QString(mount_point.data()) ;
}

void zuluCrypt::sovfinished()
{
	delete sov ;
}

void zuluCrypt::closeEvent(QCloseEvent *e)
{
	if(trayIcon->isVisible() == true){
		this->hide();
		e->ignore();
	}else{
		this->hide();
		e->accept();
	}
}

void zuluCrypt::closeApplication()
{
	trayIcon->hide();
	this->hide() ;
	QCoreApplication::quit();
}

void zuluCrypt::trayClicked(QSystemTrayIcon::ActivationReason e)
{
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

	f.open(QIODevice::ReadOnly) ;

	QByteArray c = f.readAll() ;

	f.close();

	f.open(QIODevice::WriteOnly | QIODevice::Truncate) ;

	QByteArray data ;
	
	if(c.at(0) == '1'){
		data.append('0') ;
		f.write(data) ;
		trayIcon->hide();
	}else{
		data.append('1') ;
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
	QString info = tr("cryptographic options used in volume management\n\n") ;

	info = info + tr("type:\t\tplain\n") ;
	info = info + tr("cipher:\t\taes-cbc-essiv:sha256\n") ;
	info = info + tr("keysize:\t\t256bits ( 32 bytes )\n") ;
	info = info + tr("hash:\t\tripemd160\n") ;

	info = info + QString("\n") ;

	info = info + tr("type:\t\tluks\n") ;
	info = info + tr("keysize:\t\t256bits ( 32 bytes )\n\n") ;
	info = info + tr("luks header hashing: sha1\n\n") ;

	info = info + tr("key files are generated with:\t\t64 characters( 512bits )\n\n") ;

	info = info +tr("random number generator device:\t( user specified )\n") ;

	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.addButton(QMessageBox::Ok);
	m.setWindowTitle(tr("cryptographic info"));
	m.setFont(this->font());
	m.setText(info) ;

	m.exec() ;
}

void zuluCrypt::createEncryptedpartitionUI()
{
	emit showNonSystemPartitions( ) ;
}

QStringList zuluCrypt::luksEmptySlots(QString volumePath)
{
	QProcess N ;

	N.start(QString(ZULUCRYPTzuluCrypt) + \
		QString(" emptyslots \"") + volumePath + QString("\""));

	N.waitForFinished() ;

	QByteArray s = N.readAllStandardOutput() ;

	int i = 0 ;

	for ( int j = 0 ; j < s.size() ; j++){
		if( s.at(j) == '1' || s.at(j) == '3' )
			i++ ;
	}

	N.close();

	QStringList l ;

	l << QString::number( i ) ;

	l << QString::number(  s.size() - 1 ) ;

	return l ;
}

bool zuluCrypt::isLuks(QString volumePath)
{
	QProcess N ;

	N.start(QString(ZULUCRYPTzuluCrypt) + \
		QString(" isLuks ") + QString("\"") + volumePath + QString("\"") );

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
	m.setWindowTitle(tr("about zuluCrypt"));
	m.setFont(this->font());

	QString b = tr("\nThis program is distributed in the hope that it will be useful,") ;
	b = b + tr("but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY ");
	b = b + tr("or FITNESS FOR A PARTICULAR PURPOSE.\n");
	b = b + tr("See the GNU General Public License for more details.") ;

	m.setText( tr("version") + QString(" ") + QString( VERSION_STRING ) + \
		tr(" of zuluCrypt, a front end to cryptsetup.\n\n(c) 2011 mhogo mchungu\nmhogomchungu@gmail.com\n") + b ) ;
	m.exec() ;
}

void zuluCrypt::addItemToTable(QString x,QString y)
{
	addThread = new addItemToTableThread(&mutex,ui->tableWidget,x,y,&item_count,&selectedRow) ;

	connect(addThread,
		SIGNAL(threadFinished(addItemToTableThread*)),
		this,
		SLOT(deleteAddItemToTableThread(addItemToTableThread *))) ;

	addThread->start();
}

void zuluCrypt::deleteAddItemToTableThread(addItemToTableThread *c)
{
	//sleep(3) ;
	delete c ;
}

void zuluCrypt::removeRowFromTable( int x )
{
	ui->tableWidget->removeRow( x ) ;

	item_count-- ;
}

void zuluCrypt::volume_property()
{
	vpt->update(item->tableWidget()->item(item->row(),0)->text(),
		    item->tableWidget()->item(item->row(),1)->text(),
		    &volumeProperty);

	vpt->start();
}

void zuluCrypt::volumePropertyThreadFinished()
{
	mpv.setText(volumeProperty);
	mpv.exec() ;
}

void zuluCrypt::favAboutToHide()
{
	ui->menuFavorites->clear();
}

void zuluCrypt::favClicked(QAction *e)
{
	QMenu *m = (QMenu *) e->parentWidget() ;

	if( e->text() == tr("open") ){

		QStringList l =  m->title().split("\t") ;

		emit favClickedVolume(l.at(0),l.at(1));

	}else if( e->text() == tr("remove from favorite") ){

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

		//menulist.append(m);
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

void zuluCrypt::cellClicked(QTableWidgetItem * t)
{
	item = t ;	

	crh->update( item->row(),ui->tableWidget,&selectedRow,item_count );

	crh->start();

	QMenu m ;
	m.setFont(this->font());
	connect(m.addAction("close"),SIGNAL(triggered()),this,SLOT(close())) ;

	m.addSeparator() ;

	connect(m.addAction("properties"),SIGNAL(triggered()),this,SLOT(volume_property())) ;

	if( ui->tableWidget->item(item->row(),2)->text() == QString("luks") ){
		m.addSeparator() ;
		connect(m.addAction("add key"),
			SIGNAL(triggered()),
			this,
			SLOT(luksAddKeyContextMenu())) ;
		connect(m.addAction("remove key"),
			SIGNAL(triggered()),
			this,
			SLOT(luksDeleteKeyContextMenu())) ;
	}
	
	m.addSeparator() ;

	QString volume_path = ui->tableWidget->item(item->row(),0)->text() ;
	QString mount_point_path = ui->tableWidget->item(item->row(),1)->text();

	int i = mount_point_path.lastIndexOf("/") ;

	mount_point_path = mount_point_path.left( i ) ;

	QString fav = volume_path + QString("\t") + mount_point_path ;

	QFile f(QDir::homePath() + QString("/.zuluCrypt/favorites")) ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray data = f.readAll() ;

	QAction a(tr("add to favorite"),(QObject *)&m) ;

	if( strstr( data.data() , fav.toAscii().data() ) == NULL )
		a.setEnabled(true);
	else
		a.setEnabled(false);

	a.connect((QObject *)&a,
		  SIGNAL(triggered()),
		  this,
		  SLOT(addToFavorite())) ;

	m.addAction(&a);
	
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

void zuluCrypt::closeThreadFinished()
{
	ui->tableWidget->setEnabled( true );

	delete vct ;

	switch ( status ) {
	case 0 :removeRowFromTable(item->row()) ;

		if( item_count > 0 ){

			selectedRow = 0 ;

			ui->tableWidget->item(0,0)->setSelected(true);
			ui->tableWidget->item(0,1)->setSelected(true);
			ui->tableWidget->item(0,2)->setSelected(true);
		}else
			selectedRow = -1 ;

		break ;
	case 1 :UIMessage(tr("ERROR"),
			  tr("close failed, encrypted volume with that name does not exist")) ;
		break ;
	case 2 :UIMessage(tr("ERROR"),
			  tr("close failed, the mount point and/or one or more files are in use"));
		break ;
	case 3 :UIMessage(tr("ERROR"),
			  tr("close failed, volume does not have an entry in /etc/mtab"));
		break ;
	case 4 :UIMessage(tr("ERROR"),
			  tr("close failed, could not get a lock on /etc/mtab~"));
		break ;
	default :UIMessage(tr("ERROR"),
			  tr("an unknown error has occured, volume not closed"));
	}
}

void zuluCrypt::close()
{
	QProcess p ;

	QString vol = ui->tableWidget->item(item->row(),0)->text().replace("\"","\"\"\"") ;

	QString exe = QString(ZULUCRYPTzuluCrypt) + QString(" close ") + QString("\"") + \
			vol + QString("\"") ;

	vct = new runInThread( exe, &status,NULL) ;

	connect(vct,
		SIGNAL(finished()),
		this,
		SLOT(closeThreadFinished())) ;

	ui->tableWidget->setEnabled( false );
	vct->start();
}

void zuluCrypt::setupUIElements()
{
	mpv.setWindowTitle(tr("volume properties"));
	mpv.setParent(this);
	mpv.setWindowFlags(Qt::Window | Qt::Dialog);
	mpv.addButton(QMessageBox::Ok);
	mpv.setFont(this->font());

	trayIcon = new QSystemTrayIcon(this);

	trayIcon->setIcon(QIcon(QString(":/zuluCrypt.png")));

	trayMenu = new QMenu(this) ;

	trayMenu->addAction(tr("quit"),
			    this,SLOT(closeApplication()));

	trayIcon->setContextMenu(trayMenu);

	ui->setupUi(this);

	//ui->tableWidget->setMouseTracking(true);

	this->setFixedSize(this->size());

	this->setWindowIcon(QIcon(QString(":/zuluCrypt.png")));

	ui->tableWidget->setColumnWidth(0,230);

	ui->tableWidget->setColumnWidth(1,290);

	ui->tableWidget->setColumnWidth(2,90);

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
}

void zuluCrypt::setupConnections()
{
	connect(ui->actionCreatekeyFile,
		SIGNAL(triggered()),
		createkeyFile,
		SLOT(ShowUI()));

	connect(createFile,
		SIGNAL(fileCreated(QString)),
		createpartitionUI,
		SLOT(ShowFileUI(QString)));

	connect(ui->actionFileCreate,
		SIGNAL(triggered()),
		createFile,
		SLOT(showUI()));

	connect(NonSystemPartitions,
		SIGNAL(clickedPartition(QString)),
		createpartitionUI,
		SLOT(ShowPartitionUI(QString)));

	connect(this,
		SIGNAL(showNonSystemPartitions()),
		NonSystemPartitions,
		SLOT(ShowNonSystemPartitions()));

	connect(ui->actionFileOpen,
		SIGNAL(triggered()),
		openFileUI,
		SLOT(ShowUI())) ;

	connect(ui->actionPartitionOpen,
		SIGNAL(triggered()),
		openPartitionUI,
		SLOT(ShowUI()));

	connect(ui->tableWidget,
		SIGNAL(itemClicked(QTableWidgetItem*)),
		this,
		SLOT(cellClicked(QTableWidgetItem*))) ;

	connect(openPartitionUI,
		SIGNAL(clickedPartition(QString)),
		openFileUI,
		SLOT(clickedPartitionOption(QString)));

	connect(ui->actionAbout,
		SIGNAL(triggered()),
		this,
		SLOT(aboutMenuOption())) ;

	connect(ui->actionAddKey,
		SIGNAL(triggered()),
		addKeyUI,
		SLOT(ShowUI()) ) ;

	connect(ui->actionDeleteKey,
		SIGNAL(triggered()),
		deleteKeyUI,
		SLOT(ShowUI()) ) ;

	connect(ui->actionPartitionCreate,
		SIGNAL(triggered()),
		this,
		SLOT(createEncryptedpartitionUI())) ;

	connect(ui->actionInfo,
		SIGNAL(triggered()),
		this,
		SLOT(info())) ;

	connect(ui->actionFonts,
		SIGNAL(triggered()),
		this,
		SLOT(fonts())) ;

	connect(ui->menuFavorites,
		SIGNAL(aboutToShow()),
		this,
		SLOT(readFavorites())) ;

	connect(ui->menuFavorites,
		SIGNAL(aboutToHide()),
		this,
		SLOT(favAboutToHide())) ;

	connect(ui->actionTray_icon,
		SIGNAL(triggered()),
		this,
		SLOT(trayProperty())) ;

	connect(this,
		SIGNAL(favClickedVolume(QString,QString)),
		openFileUI,
		SLOT(ShowUI(QString,QString))) ;

	connect(trayIcon,
		SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this,
		SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));

	connect(ui->menuFavorites,
		SIGNAL(triggered(QAction*)),
		this,
		SLOT(favClicked(QAction*))) ;

	connect(openFileUI,
		SIGNAL(addItemToTable(QString,QString)),
		this,
		SLOT(addItemToTable(QString,QString))) ;

	connect(this,
		SIGNAL(luksAddKeyUI(QString)),
		addKeyUI,
		SLOT(partitionEntry(QString))) ;

	connect(this,
		SIGNAL(luksDeleteKeyUI(QString)),
		deleteKeyUI,
		SLOT(deleteKey(QString)));

	connect(ui->action_close,
		SIGNAL(triggered()),
		this,
		SLOT(closeApplication())) ;

	connect(ui->action_minimize,
		SIGNAL(triggered()),
		this,
		SLOT(minimize()));

	connect(ui->actionMinimize_to_tray,
		SIGNAL(triggered()),
		this,
		SLOT(minimizeToTray())) ;

	connect(ui->actionClose_all_opened_volumes,
		SIGNAL(triggered()),
		this,
		SLOT(closeAllVolumes())) ;

	//connect(ui->tableWidget,
	//	SIGNAL(itemClicked(QTableWidgetItem*)),
	//	this,
	//	SLOT(cellEntered(QTableWidgetItem*))) ;
}

zuluCrypt::~zuluCrypt()
{	
	//for( int i = 0 ; i < menulist.size() ; i++){

	//	delete menulist.at(i) ;
	//}

	//menulist.clear();

	delete openFileUI ;

	delete openPartitionUI ;

	delete NonSystemPartitions ;

	delete luksopenPartitionUI ;

	delete createpartitionUI ;

	delete addKeyUI ;

	delete deleteKeyUI ;

	delete createFile ;

	delete createkeyFile ;

	delete trayMenu ;

	delete  crh ;

	delete vpt ;

	delete ui;
}
