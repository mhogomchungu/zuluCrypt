/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
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
#include "miscfunctions.h"

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
#include <QKeySequence>

zuluCrypt::zuluCrypt(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::zuluCrypt)
{
	setupUIElements();
	setupConnections();
	StartUpAddOpenedVolumesToTableThread();
	initTray();
	initFont();
	initKeyCombo();
}

void zuluCrypt::initKeyCombo()
{
	QAction * rca = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Menu );
	keys.append( Qt::CTRL + Qt::Key_M );
	rca->setShortcuts(keys) ;
	connect(rca,SIGNAL(triggered()),this,SLOT(menuKeyPressed())) ;
	this->addAction( rca );
}

void zuluCrypt::initFont()
{
	QString fontPath = QDir::homePath() + QString("/.zuluCrypt/font") ;
	QFile z( fontPath ) ;
	if(z.exists() == false){
		z.open(QIODevice::WriteOnly | QIODevice::Truncate) ;
		QString s = QString("Sans Serif\n8\nnormal\nnormal\n") ;
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
}

void zuluCrypt::initTray()
{
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
	m_ui->actionTray_icon->setCheckable(true);
	if( c.at(0) == '1'){
		m_ui->actionTray_icon->setChecked(true);
		m_trayIcon->show();
	}else{
		m_ui->actionTray_icon->setChecked(false);
		m_trayIcon->hide();
	}
}

void zuluCrypt::startUpdateFinished()
{
	m_ui->tableWidget->setEnabled( true );
	m_ui->tableWidget->setFocus();
}

void zuluCrypt::StartUpAddOpenedVolumesToTableThread()
{
	m_ui->tableWidget->setEnabled( false );
	startupupdateopenedvolumes * sov = new startupupdateopenedvolumes();
	connect(sov,
	       SIGNAL(addItemToTable(QString,QString)),
	       this,
	       SLOT(addItemToTable(QString,QString))) ;
	connect(sov,
		SIGNAL(finished()),
		this,SLOT(startUpdateFinished()));
	connect(sov,
	       SIGNAL(UIMessage(QString,QString)),
	       this,
	       SLOT(UIMessage(QString,QString))) ;
	QThreadPool::globalInstance()->start(sov);
}

void zuluCrypt::setupUIElements()
{
	m_trayIcon = new QSystemTrayIcon(this);
	m_trayIcon->setIcon(QIcon(QString(":/zuluCrypt.png")));

	QMenu *trayMenu = new QMenu(this) ;
	trayMenu->addAction(tr("quit"),
			    this,SLOT(closeApplication()));
	m_trayIcon->setContextMenu(trayMenu);

	m_ui->setupUi(this);

	this->setFixedSize(this->size());
	this->setWindowIcon(QIcon(QString(":/zuluCrypt.png")));

	m_ui->tableWidget->setColumnWidth(0,298);
	m_ui->tableWidget->setColumnWidth(1,290);
	m_ui->tableWidget->setColumnWidth(2,90);
}

void zuluCrypt::setupConnections()
{
	connect(this,SIGNAL(favClickedVolume(QString,QString)),	this,
		SLOT(ShowPasswordDialogFromFavorite(QString,QString))) ;
	connect(m_ui->actionPartitionOpen,SIGNAL(triggered()),this,SLOT(ShowOpenPartition()));
	connect(m_ui->actionFileOpen,SIGNAL(triggered()),this,SLOT(ShowPasswordDialog())) ;
	connect(m_ui->actionFileCreate,SIGNAL(triggered()),this,SLOT(ShowCreateFile()));
	connect(m_ui->actionManage_names,SIGNAL(triggered()),this,SLOT(ShowFavoritesEntries()));
	connect(m_ui->tableWidget,
		SIGNAL(currentItemChanged( QTableWidgetItem * , QTableWidgetItem * )),
		this,
		SLOT(currentItemChanged( QTableWidgetItem * , QTableWidgetItem * ))) ;
	connect(m_ui->actionCreatekeyFile,SIGNAL(triggered()),this,SLOT(ShowCreateKeyFile()));
	connect(m_ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(itemClicked(QTableWidgetItem*))) ;
	connect(m_ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutMenuOption())) ;
	connect(m_ui->actionAddKey,SIGNAL(triggered()),this,SLOT(ShowAddKey()) ) ;
	connect(m_ui->actionDeleteKey,SIGNAL(triggered()),this,SLOT(ShowDeleteKey()) ) ;
	connect(m_ui->actionPartitionCreate,SIGNAL(triggered()),this,SLOT(ShowNonSystemPartitions())) ;
	connect(m_ui->actionInfo,SIGNAL(triggered()),this,SLOT(info())) ;
	connect(m_ui->actionFonts,SIGNAL(triggered()),this,SLOT(fonts())) ;
	connect(m_ui->menuFavorites,SIGNAL(aboutToShow()),this,SLOT(readFavorites())) ;
	connect(m_ui->menuFavorites,SIGNAL(aboutToHide()),this,SLOT(favAboutToHide())) ;
	connect(m_ui->actionTray_icon,SIGNAL(triggered()),this,SLOT(trayProperty())) ;
	connect(m_trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,
		SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
	connect(m_ui->menuFavorites,SIGNAL(triggered(QAction*)),this,SLOT(favClicked(QAction*))) ;
	connect(this,SIGNAL(luksAddKey(QString)),this,SLOT(ShowAddKeyContextMenu(QString))) ;
	connect(this,SIGNAL(luksDeleteKey(QString)),this,SLOT(ShowDeleteKeyContextMenu(QString)));
	connect(m_ui->action_close,SIGNAL(triggered()),this,SLOT(closeApplication())) ;
	connect(m_ui->action_minimize,SIGNAL(triggered()),this,SLOT(minimize()));
	connect(m_ui->actionMinimize_to_tray,SIGNAL(triggered()),this,SLOT(minimizeToTray())) ;
	connect(m_ui->actionClose_all_opened_volumes,SIGNAL(triggered()),this,SLOT(closeAllVolumes())) ;
}

void zuluCrypt::HighlightRow(int row,bool b)
{
	for( int i = 0 ; i < 3 ; i++)
		m_ui->tableWidget->item(row,i)->setSelected(b);
	if( b == true)
		m_ui->tableWidget->setCurrentCell(row,1);
	m_ui->tableWidget->setFocus();
}

void zuluCrypt::currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
	if(current != NULL)
		HighlightRow(current->row(), true) ;
	if(previous != NULL)
		if(current != NULL)
			if(previous->row() != current->row())
				HighlightRow(previous->row(), false) ;
}

void zuluCrypt::closeAllVolumes()
{
	if( m_ui->tableWidget->rowCount() < 1 )
		return ;
	closeAllVolumesThread *cavt = new closeAllVolumesThread(m_ui->tableWidget) ;
	connect(cavt,
		SIGNAL(close(QTableWidgetItem *,int)),
		this,
		SLOT(closeAll(QTableWidgetItem *,int))) ;

	QThreadPool::globalInstance()->start(cavt);	
}

void zuluCrypt::closeAll(QTableWidgetItem * i,int st)
{
	if( st == 0 )
		removeRowFromTable(i->row());
	else		
		closeStatusErrorMessage(st);	
}

void zuluCrypt::minimize()
{
	zuluCrypt::setWindowState(Qt::WindowMinimized) ;
}

void zuluCrypt::minimizeToTray()
{
	if( m_ui->actionTray_icon->isChecked() == true ){
		this->hide();
	}else{
		m_ui->actionTray_icon->setChecked(true);
		trayProperty() ;
		this->hide();
	}
}

void zuluCrypt::closeEvent(QCloseEvent *e)
{
	if(m_trayIcon->isVisible() == true){
		this->hide();
		e->ignore();
	}else{
		this->hide();
		e->accept();
	}
}

void zuluCrypt::closeApplication()
{
	m_trayIcon->hide();
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
		m_trayIcon->hide();
	}else{
		data.append('1') ;
		f.write(data) ;
		m_trayIcon->show();
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

	m_ui->tableWidget->horizontalHeaderItem(0)->setFont(Font);
	m_ui->tableWidget->horizontalHeaderItem(1)->setFont(Font);
	m_ui->tableWidget->horizontalHeaderItem(2)->setFont(Font);

	m_ui->actionAbout->setFont(this->font());
	m_ui->actionAddKey->setFont(this->font());
	m_ui->actionCreatekeyFile->setFont(this->font());
	m_ui->actionDeleteKey->setFont(this->font());
	m_ui->actionFavorite_volumes->setFont(this->font());
	m_ui->actionFileCreate->setFont(this->font());
	m_ui->actionFileOpen->setFont(this->font());
	m_ui->actionFonts->setFont(this->font());
	m_ui->actionInfo->setFont(this->font());
	m_ui->actionManage_favorites->setFont(this->font());
	m_ui->actionPartitionCreate->setFont(this->font());
	m_ui->actionPartitionOpen->setFont(this->font());
	m_ui->actionSelect_random_number_generator->setFont(this->font());
	m_ui->actionTray_icon->setFont(this->font());
	m_ui->menuFavorites->setFont(this->font());
	m_ui->actionManage_names->setFont(this->font());
	m_ui->menu_zc->setFont(this->font());
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

	UIMessage(tr("cryptographic info"),info);
}

void zuluCrypt::createEncryptedpartitionUI()
{
	emit SignalShowNonSystemPartitions( ) ;
}

void zuluCrypt::aboutMenuOption(void)
{
	QString license = QString("version %1 of zuluCrypt, a front end to cryptsetup.\n\n\
name : mhogo mchungu\n\
Copyright 2011,2012\n\
email: mhogomchungu@gmail.com\n\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 2 of the License, or\n\
( at your option ) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>.").arg(VERSION_STRING);

	UIMessage(tr("about zuluCrypt"),license);
}

//void zuluCrypt::addItemToTableByVolume(QString vp)
//{
//	QString zvp = QString("/dev/mapper/zuluCrypt-") + vp.split("/").last() ;
//	addItemToTable(vp, miscfunctions::mtab(zvp));
//}

void zuluCrypt::addItemToTable(QString device,QString m_point)
{
	miscfunctions::addItemToTable(m_ui->tableWidget,device,m_point);
}

void zuluCrypt::removeRowFromTable( int x )
{
	m_ui->tableWidget->removeRow( x ) ;
	int count = m_ui->tableWidget->rowCount() ;
	if( count > 0 )
		m_ui->tableWidget->setCurrentCell(count - 1,1);
}

void zuluCrypt::volume_property()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	QString x = m_ui->tableWidget->item(item->row(),0)->text() ;
	QString y = m_ui->tableWidget->item(item->row(),1)->text() ;

	volumePropertiesThread * vpt = new volumePropertiesThread(x,y);

	connect(vpt,
		SIGNAL(finished(QString)),
		this,
		SLOT(volumePropertyThreadFinished(QString))) ;

	QThreadPool::globalInstance()->start(vpt);
}

void zuluCrypt::volumePropertyThreadFinished(QString properties)
{
	UIMessage(tr("volume properties"),properties);
}

void zuluCrypt::favAboutToHide()
{
}

void zuluCrypt::favClicked(QAction *e)
{
	QStringList l = e->text().split("\t") ;
	emit favClickedVolume(l.at(0),l.at(1));
}

void zuluCrypt::readFavorites()
{
	m_ui->menuFavorites->clear();
	QStringList l = miscfunctions::readFavorites() ;
	for(int i = 0 ; i < l.size() - 1 ; i++)
		m_ui->menuFavorites->addAction(new QAction(l.at(i),m_ui->menuFavorites));
}

void zuluCrypt::addToFavorite()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	miscfunctions::addToFavorite(m_ui->tableWidget->item(item->row(),0)->text(),
				     m_ui->tableWidget->item(item->row(),1)->text());
}

void zuluCrypt::menuKeyPressed()
{
	QTableWidgetItem *it = m_ui->tableWidget->currentItem() ;
	if( it != NULL )
		itemClicked( it,false );
}

void zuluCrypt::itemClicked(QTableWidgetItem * it)
{
	if( it != NULL )
		itemClicked( it,true );
}

void zuluCrypt::itemClicked(QTableWidgetItem * item, bool clicked)
{
	QMenu m ;
	m.setFont(this->font());
	connect(m.addAction("close"),SIGNAL(triggered()),this,SLOT(close())) ;

	m.addSeparator() ;

	connect(m.addAction("properties"),SIGNAL(triggered()),this,SLOT(volume_property())) ;

	if( m_ui->tableWidget->item(item->row(),2)->text() == QString("luks") ){
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

	QString volume_path = m_ui->tableWidget->item(item->row(),0)->text() ;
	QString mount_point_path = m_ui->tableWidget->item(item->row(),1)->text();

	int i = mount_point_path.lastIndexOf("/") ;

	mount_point_path = mount_point_path.left( i ) ;

	QString fav = volume_path + QString("\t") + mount_point_path ;

	QFile f(QDir::homePath() + QString("/.zuluCrypt/favorites")) ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray data = f.readAll() ;

	QAction a(tr("add to favorite"),(QObject *)&m) ;

	m.addAction(&a);

	if( strstr( data.data() , fav.toAscii().data() ) == NULL ){
		a.setEnabled(true);
		a.connect((QObject *)&a,
			  SIGNAL(triggered()),
			  this,
			  SLOT(addToFavorite())) ;

	}else
		a.setEnabled(false);

	if( clicked == true )
		m.exec(QCursor::pos()) ;
	else{
		int x = m_ui->tableWidget->columnWidth(0) ;
		int y = m_ui->tableWidget->rowHeight(item->row()) * item->row() + 20 ;

		m.addSeparator() ;
		m.addAction("cancel") ;
		m.exec(m_ui->tableWidget->mapToGlobal(QPoint(x,y))) ;
	}
}

void zuluCrypt::luksAddKeyContextMenu(void)
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	emit luksAddKey(m_ui->tableWidget->item(item->row(),0)->text() ) ;
}

void zuluCrypt::luksDeleteKeyContextMenu(void)
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	emit luksDeleteKey(m_ui->tableWidget->item(item->row(),0)->text()) ;
}

void zuluCrypt::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setParent(this);
	m.setFont(this->font());
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.exec() ;
}

void zuluCrypt::closeStatus(int st)
{
	m_ui->tableWidget->setEnabled( true );
	if( st == 0 )
		removeRowFromTable(m_ui->tableWidget->currentItem()->row()) ;
	else
		closeStatusErrorMessage(st);
}

void zuluCrypt::closeStatusErrorMessage(int st)
{
	switch ( st ) {
	case 1 :UIMessage(tr("ERROR"),
			  tr("close failed, encrypted volume with that name does not exist")) ;
		break ;
	case 2 :UIMessage(tr("ERROR"),
			  tr("close failed, the mount point and/or one or more files from the volume are in use."));
		break ;
	case 3 :UIMessage(tr("ERROR"),
			  tr("close failed, volume does not have an entry in /etc/mtab"));
		break ;
	case 4 :UIMessage(tr("ERROR"),
			  tr("close failed, could not get a lock on /etc/mtab~"));
		break ;	
	case 5 :UIMessage(tr("ERROR"),
		tr("close failed, volume is unmounted but could not close mapper,advice to close it manually"));
		break ;	
	case 11 :UIMessage(tr("ERROR"),
		tr("could not find any partition with the presented UUID"));
		break ;
	default :UIMessage(tr("ERROR"),
			  tr("an unknown error has occured, volume not closed"));
	}
}

void zuluCrypt::close()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	QString vol = m_ui->tableWidget->item(item->row(),0)->text().replace("\"","\"\"\"") ;
	QString exe = QString(ZULUCRYPTzuluCrypt) + QString(" close ") + QString("\"") + \
			vol + QString("\"") ;
	m_ui->tableWidget->setEnabled( false );
	closeVolumeThread * cvt = new closeVolumeThread( exe ) ;
	connect(cvt,SIGNAL(finished(int)),this,SLOT(closeStatus(int))) ;
	QThreadPool::globalInstance()->start(cvt);
}

luksaddkey * zuluCrypt::setUpluksaddkey()
{
	luksaddkey *addKey = new luksaddkey(this);
	connect(addKey,SIGNAL(HideUISignal()),addKey,SLOT(deleteLater()));
	return addKey ;
}

void zuluCrypt::ShowAddKeyContextMenu(QString key)
{
	setUpluksaddkey()->partitionEntry(key);
}

void zuluCrypt::ShowAddKey()
{
	setUpluksaddkey()->ShowUI();
}

luksdeletekey * zuluCrypt::setUpluksdeletekey()
{
	luksdeletekey *deleteKey = new luksdeletekey(this);
	connect(deleteKey,SIGNAL(HideUISignal()),deleteKey,SLOT(deleteLater()));
	return deleteKey ;
}

void zuluCrypt::ShowDeleteKeyContextMenu(QString key)
{
	setUpluksdeletekey()->deleteKey( key );
}

void zuluCrypt::ShowDeleteKey()
{
	setUpluksdeletekey()->ShowUI();
}

void zuluCrypt::ShowCreateKeyFile()
{
	createkeyfile * ckf = new createkeyfile(this);
	connect(ckf,SIGNAL(HideUISignal()),ckf,SLOT(deleteLater()));
	ckf->ShowUI();
}

void zuluCrypt::ShowFavoritesEntries()
{
	managedevicenames * mdn = new managedevicenames(this);
	connect(mdn,SIGNAL(HideUISignal()),mdn,SLOT(deleteLater()));
	mdn->ShowUI();
}

void zuluCrypt::ShowCreateFile()
{
	createfile * cf = new createfile(this);
	connect(cf,SIGNAL(HideUISignal()),cf,SLOT(deleteLater()));
	connect(cf,SIGNAL(fileCreated(QString)),this,SLOT(FileCreated(QString)));
	cf->showUI();
}

createpartition * zuluCrypt::setUpCreatepartition()
{
	createpartition * cp = new createpartition(this);
	connect(cp,SIGNAL(HideUISignal()),cp,SLOT(deleteLater()));
	return cp;
}

void zuluCrypt::createPartition(QString partition)
{
	setUpCreatepartition()->ShowPartition(partition);
}

void zuluCrypt::FileCreated(QString file)
{
	setUpCreatepartition()->ShowFile(file);
}

openpartition * zuluCrypt::setUpOpenpartition()
{
	openpartition * op = new openpartition(this);
	connect(op,SIGNAL(HideUISignal()),op,SLOT(deleteLater()));
	return op ;
}

void zuluCrypt::ShowNonSystemPartitions()
{
	openpartition * nsp = setUpOpenpartition() ;
	connect(nsp,SIGNAL(clickedPartition(QString)),this,SLOT(createPartition(QString)));
	nsp->ShowNonSystemPartitions();
}

void zuluCrypt::ShowOpenPartition()
{
	openpartition * ap = setUpOpenpartition() ;
	connect(ap,SIGNAL(clickedPartition(QString)),this,SLOT(partitionClicked(QString)));
	ap->ShowAllPartitions();
}

passwordDialog * zuluCrypt::setUpPasswordDialog()
{
	passwordDialog * pd = new passwordDialog(m_ui->tableWidget,this) ;
	connect(pd,SIGNAL(HideUISignal()),pd,SLOT(deleteLater()));
	return pd ;
}

void zuluCrypt::ShowPasswordDialog()
{
	setUpPasswordDialog()->ShowUI();
}

void zuluCrypt::ShowPasswordDialogFromFavorite(QString x, QString y)
{
	setUpPasswordDialog()->ShowUI(x,y);
}

void zuluCrypt::partitionClicked(QString partition)
{
	setUpPasswordDialog()->clickedPartitionOption(partition);
}

zuluCrypt::~zuluCrypt()
{
	delete m_ui;
	delete m_trayIcon;
}
