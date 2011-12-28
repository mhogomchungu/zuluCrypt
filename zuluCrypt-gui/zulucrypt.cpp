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

Q_DECLARE_METATYPE(Qt::Orientation) ;
Q_DECLARE_METATYPE(QItemSelection) ;

zuluCrypt::zuluCrypt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::zuluCrypt)
{
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

	vpt = new volumePropertiesThread() ;

	connect(vpt,
		SIGNAL(finished()),
		this,
	SLOT(volumePropertyThreadFinished())) ;
	 /*
	startupupdateopenedvolumes sov = new startupupdateopenedvolumes(this);
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
*/
	rca = new QAction( this ) ;

	QList<QKeySequence> keys ;

	keys.append( Qt::Key_Menu );
	keys.append( Qt::CTRL + Qt::Key_M );

	rca->setShortcuts(keys) ;
	connect(rca,SIGNAL(triggered()),this,SLOT(menuKeyPressed())) ;

	this->addAction( rca );

	StartUpAddOpenedVolumesToTable() ;
}

void zuluCrypt::setupUIElements()
{
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(QString(":/zuluCrypt.png")));

	QMenu *trayMenu = new QMenu(this) ;
	trayMenu->addAction(tr("quit"),
			    this,SLOT(closeApplication()));
	trayIcon->setContextMenu(trayMenu);

	ui->setupUi(this);

	this->setFixedSize(this->size());
	this->setWindowIcon(QIcon(QString(":/zuluCrypt.png")));

	ui->tableWidget->setColumnWidth(0,298);
	ui->tableWidget->setColumnWidth(1,290);
	ui->tableWidget->setColumnWidth(2,90);
}

void zuluCrypt::setupConnections()
{
	connect(this,SIGNAL(favClickedVolume(QString,QString)),	this,
		SLOT(ShowPasswordDialogUIFromFavorite(QString,QString))) ;
	connect(ui->actionPartitionOpen,SIGNAL(triggered()),this,SLOT(ShowOpenPartitionUI()));
	connect(ui->actionFileOpen,SIGNAL(triggered()),this,SLOT(ShowPasswordDialogUI())) ;
	connect(ui->actionFileCreate,SIGNAL(triggered()),this,SLOT(ShowCreateFileUI()));
	connect(ui->actionManage_names,SIGNAL(triggered()),this,SLOT(showManageFavoritesUI()));
	connect(ui->tableWidget,
		SIGNAL(currentItemChanged( QTableWidgetItem * , QTableWidgetItem * )),
		this,
		SLOT(currentItemChanged( QTableWidgetItem * , QTableWidgetItem * ))) ;
	connect(ui->actionCreatekeyFile,SIGNAL(triggered()),this,SLOT(ShowCreateKeyFileUI()));
	//connect(this,SIGNAL(SignalShowNonSystemPartitions()),this,SLOT(ShowNonSystemPartitions()));
	connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(itemClicked(QTableWidgetItem*))) ;
	connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutMenuOption())) ;
	connect(ui->actionAddKey,SIGNAL(triggered()),this,SLOT(ShowAddKeyUI()) ) ;
	connect(ui->actionDeleteKey,SIGNAL(triggered()),this,SLOT(ShowDeleteKeyUI()) ) ;
	connect(ui->actionPartitionCreate,SIGNAL(triggered()),this,SLOT(ShowNonSystemPartitions())) ;
	connect(ui->actionInfo,SIGNAL(triggered()),this,SLOT(info())) ;
	connect(ui->actionFonts,SIGNAL(triggered()),this,SLOT(fonts())) ;
	connect(ui->menuFavorites,SIGNAL(aboutToShow()),this,SLOT(readFavorites())) ;
	connect(ui->menuFavorites,SIGNAL(aboutToHide()),this,SLOT(favAboutToHide())) ;
	connect(ui->actionTray_icon,SIGNAL(triggered()),this,SLOT(trayProperty())) ;
	connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,
		SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
	connect(ui->menuFavorites,SIGNAL(triggered(QAction*)),this,SLOT(favClicked(QAction*))) ;
	connect(this,SIGNAL(luksAddKeyUI(QString)),this,SLOT(ShowAddKeyContextMenu(QString))) ;
	connect(this,SIGNAL(luksDeleteKeyUI(QString)),this,SLOT(ShowDeleteKeyContextMenu(QString)));
	connect(ui->action_close,SIGNAL(triggered()),this,SLOT(closeApplication())) ;
	connect(ui->action_minimize,SIGNAL(triggered()),this,SLOT(minimize()));
	connect(ui->actionMinimize_to_tray,SIGNAL(triggered()),this,SLOT(minimizeToTray())) ;
	connect(ui->actionClose_all_opened_volumes,SIGNAL(triggered()),this,SLOT(closeAllVolumes())) ;
}

bool zuluCrypt::checkUUID(QString *uuid, QString entry)
{	
	if(entry.mid(0,15) == QString("zuluCrypt-UUID-")){
		*uuid = QString("UUID=\"") + entry.mid(15)  + QString("\"");
		return true ;
	}
	return false ;
}

void zuluCrypt::StartUpAddOpenedVolumesToTable()
{
	QStringList Z =  QDir(QString("/dev/mapper")).entryList().filter("zuluCrypt-") ;
	QProcess p ;
	QString device ;
	QString dv = QString(ZULUCRYPTzuluCrypt) + QString(" device ") ;
	QString ddv = QString(ZULUCRYPTzuluCrypt) + QString(" checkUUID ") ;
	QString mp ;
	QString uuid ;

	QFile mt(QString("/etc/mtab")) ;
	mt.open(QIODevice::ReadOnly) ;
	QByteArray mtab = mt.readAll() ;
	mt.close();
	QString entry ;
	for ( int i = 0 ; i < Z.size() ; i++){
		entry = Z.at(i);
		if(checkUUID(&device,entry) == false){
			p.start( dv + entry ) ;
			p.waitForFinished() ;
			if( p.exitCode() == 1 ){
				QString s = tr("An inconsitency is detected, skipping /dev/mapper/zuluCrypt-") ;
				s = s + entry ;
				s = s + tr(" because it does not look like a cryptsetup volume") ;

				UIMessage(tr("WARNING"), s ) ;
				continue ;
			}
			device = QString(p.readAllStandardOutput()).remove('\n')  ;
			p.close();
		}else{
			p.start(ddv + device);
			p.waitForFinished() ;
			if( p.exitCode() == 1 ){
				QString s = tr("An inconsitency is detected, skipping /dev/mapper/zuluCrypt-") ;
				s = s + entry ;
				s = s + tr(" because the UUID does not match any attached partition") ;
				UIMessage(tr("WARNING"), s ) ;
				continue ;
			}
		}
		mp = miscfunctions::readMtab(&mtab,entry) ;
		if( mp == QString("") ){
			UIMessage(tr("WARNING"),
					  tr("An inconsitency is detected. Skipping \"") + \
					  device + \
					  tr("\" because its opened but not mounted"));
			continue ;
		}
		addItemToTable(device,mp) ;
	}
}

void zuluCrypt::HighlightRow(int currentRow,int previousRow)
{
	int count = ui->tableWidget->rowCount() ;
	if(count > 0 && currentRow != -1){
		ui->tableWidget->item(currentRow,0)->setSelected(true);
		ui->tableWidget->item(currentRow,1)->setSelected(true);
		ui->tableWidget->item(currentRow,2)->setSelected(true);
		ui->tableWidget->setCurrentItem(ui->tableWidget->item(currentRow,1));
	}
	if(count > 1 && currentRow != previousRow && previousRow != -1 ){
		ui->tableWidget->item(previousRow,0)->setSelected(false);
		ui->tableWidget->item(previousRow,1)->setSelected(false);
		ui->tableWidget->item(previousRow,2)->setSelected(false);
	}
	ui->tableWidget->setFocus();
}

void zuluCrypt::currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
	int c ;
	int p ;
	if( current == NULL )
		c = -1 ;
	else
		c = current->row() ;
	if( previous == NULL )
		p = -1 ;
	else
		p = previous->row() ;
	HighlightRow(c,p);
}

void zuluCrypt::closeAllVolumes()
{
	if( ui->tableWidget->rowCount() < 1 )
		return ;
	closeAllVolumesThread *cavt = new closeAllVolumesThread(ui->tableWidget) ;
	connect(cavt,SIGNAL(close(QTableWidgetItem *,int)),this,SLOT(closeAll(QTableWidgetItem *,int))) ;
	connect(cavt,SIGNAL(finished(closeAllVolumesThread*)),this,SLOT(deleteThread(closeAllVolumesThread*))) ;
	cavt->start();	 
}

void zuluCrypt::deleteThread(closeAllVolumesThread *cavt)
{
	cavt->deleteLater();
}

void zuluCrypt::closeAll(QTableWidgetItem * i,int st)
{
	if( st == 0 )
		removeRowFromTable(i->row());
	else{
		QString msg = tr("Could not close \"") + \
			      ui->tableWidget->item(i->row(),0)->text() + \
				tr("\" because the mount point and/or one or more files from the volume are in use.") ;
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

void zuluCrypt::sovfinished()
{
	//sov->deleteLater(); ;
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
	emit SignalShowNonSystemPartitions( ) ;
}

void zuluCrypt::aboutMenuOption(void)
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.addButton(QMessageBox::Ok);
	m.setWindowTitle(tr("about zuluCrypt"));
	m.setFont(this->font());

	QString license("Copyright ( c ) 2011,2012\n\
name : mhogo mchungu\n\
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
along with this program.  If not, see <http://www.gnu.org/licenses/>.") ;
	 
	m.setText( tr("version") + QString(" ") + QString( VERSION_STRING ) + \
		tr(" of zuluCrypt, a front end to cryptsetup.\n\n(c) 2011 mhogo mchungu\nmhogomchungu@gmail.com\n") + license ) ;
	m.exec() ;
}

void zuluCrypt::addItemToTableByVolume(QString vp)
{
	QString zvp = QString("/dev/mapper/zuluCrypt-") + vp.split("/").last() ;
	addItemToTable(vp, miscfunctions::mtab(zvp));
}

void zuluCrypt::addItemToTable(QString device,QString m_point)
{
	QTableWidget * tableWidget = ui->tableWidget ;
	int row = tableWidget->rowCount() ;
	tableWidget->insertRow(row);
	tableWidget->setItem(row,0,new QTableWidgetItem(device)) ;
	tableWidget->setItem(row,1,new QTableWidgetItem(m_point)) ;

	if ( miscfunctions::isLuks( device.replace("\"","\"\"\"") ) == true )
		tableWidget->setItem(row,2,new QTableWidgetItem(tr("luks"))) ;
	else
		tableWidget->setItem(row,2,new QTableWidgetItem(tr("plain"))) ;

	tableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
	tableWidget->item(row,1)->setTextAlignment(Qt::AlignCenter);
	tableWidget->item(row,2)->setTextAlignment(Qt::AlignCenter);

	tableWidget->setCurrentCell(row,1);
	/*
	addItemToTableThread * addThread  = new addItemToTableThread(&mutex,ui->tableWidget,x,y) ;
	connect(addThread,
		SIGNAL(threadFinished(addItemToTableThread*)),
		this,
		SLOT(deleteAddItemToTableThread(addItemToTableThread *))) ;
	addThread->start();
	*/

	/*
	  "passwordDialogUI->HideUI()" should ideally be in the case 0 of "threadfinished" member of
	  "password_Dialog" class. It is here because the UI freezes for a bit
	  while it disaapears when placed there.

	if(passwordDialogUI != NULL){
		passwordDialogUI->hide();
		passwordDialogUI->deleteLater();
	}*/
}

void zuluCrypt::deleteAddItemToTableThread(addItemToTableThread *Thread)
{
	Thread->deleteLater(); ;
	/*
	  "passwordDialogUI->HideUI()" should ideally be in the case 0 of "threadfinished" member of
	  "password_Dialog" class. It is here because the UI freezes for a bit
	  while it disaapears when placed there.
	  */
	//if(passwordDialogUI->isVisible() == true)
	//	passwordDialogUI->HideUI() ;
}

void zuluCrypt::removeRowFromTable( int x )
{
	ui->tableWidget->removeRow( x ) ;
	if( ui->tableWidget->rowCount() > 0 )
		ui->tableWidget->setCurrentCell(0,1);
}

void zuluCrypt::volume_property()
{
	vpt->update(ui->tableWidget->item(current_table_item->row(),0)->text(),
		    ui->tableWidget->item(current_table_item->row(),1)->text(),
		    &volumeProperty);
	vpt->start();
}

void zuluCrypt::volumePropertyThreadFinished()
{
	QMessageBox mpv ;
	mpv.setWindowTitle(tr("volume properties"));
	mpv.setParent(this);
	mpv.setWindowFlags(Qt::Window | Qt::Dialog);
	mpv.addButton(QMessageBox::Ok);
	mpv.setFont(this->font());
	mpv.setText(volumeProperty);
	mpv.exec() ;
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
	ui->menuFavorites->clear();
	QStringList l = miscfunctions::readFavorites() ;
	for(int i = 0 ; i < l.size() - 1 ; i++)
		ui->menuFavorites->addAction(new QAction(l.at(i),ui->menuFavorites));
}

void zuluCrypt::addToFavorite()
{
	miscfunctions::addToFavorite(ui->tableWidget->item(current_table_item->row(),0)->text(),
				     ui->tableWidget->item(current_table_item->row(),1)->text());
}

void zuluCrypt::menuKeyPressed()
{
	QTableWidgetItem *it = ui->tableWidget->currentItem() ;
	if( it == NULL )
		return ;
	itemClicked( it,false );
}

void zuluCrypt::itemClicked(QTableWidgetItem * it)
{
	itemClicked(it,true);
}

void zuluCrypt::itemClicked(QTableWidgetItem *it, bool clicked)
{
	current_table_item = it ;

	QMenu m ;
	m.setFont(this->font());
	connect(m.addAction("close"),SIGNAL(triggered()),this,SLOT(close())) ;

	m.addSeparator() ;

	connect(m.addAction("properties"),SIGNAL(triggered()),this,SLOT(volume_property())) ;

	if( ui->tableWidget->item(current_table_item->row(),2)->text() == QString("luks") ){
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

	QString volume_path = ui->tableWidget->item(current_table_item->row(),0)->text() ;
	QString mount_point_path = ui->tableWidget->item(current_table_item->row(),1)->text();

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
		int x = ui->tableWidget->columnWidth(0) ;
		int y = ui->tableWidget->rowHeight(current_table_item->row()) * current_table_item->row() + 20 ;

		m.addSeparator() ;
		m.addAction("cancel") ;
		m.exec(ui->tableWidget->mapToGlobal(QPoint(x,y))) ;
	}
}

void zuluCrypt::luksAddKeyContextMenu(void)
{
	emit luksAddKeyUI(ui->tableWidget->item(current_table_item->row(),0)->text() ) ;
}

void zuluCrypt::luksDeleteKeyContextMenu(void)
{
	emit luksDeleteKeyUI(ui->tableWidget->item(current_table_item->row(),0)->text()) ;
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

void zuluCrypt::closeThreadFinished(runInThread * vct,int st)
{
	ui->tableWidget->setEnabled( true );
	switch ( st ) {
	case 0 :removeRowFromTable(current_table_item->row()) ;
		break ;
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
	case 11 :UIMessage(tr("ERROR"),
		tr("could not find any partition with the presented UUID"));
		break ;
	default :UIMessage(tr("ERROR"),
			  tr("an unknown error has occured, volume not closed"));
	}
	vct->deleteLater(); ;
}

void zuluCrypt::close()
{
	QString vol = ui->tableWidget->item(current_table_item->row(),0)->text().replace("\"","\"\"\"") ;
	QString exe = QString(ZULUCRYPTzuluCrypt) + QString(" close ") + QString("\"") + \
			vol + QString("\"") ;

	runInThread * vct = new runInThread( exe ) ;

	connect(vct,
		SIGNAL(finished(runInThread *,int)),
		this,
		SLOT(closeThreadFinished(runInThread *,int))) ;
	ui->tableWidget->setEnabled( false );
	vct->start();
}

void zuluCrypt::ShowAddKeyContextMenu(QString key)
{
	luksaddkeyUI *addKeyUI = new luksaddkeyUI(this);
	addKeyUI->setWindowFlags(Qt::Window | Qt::Dialog);
	addKeyUI->setFont(this->font());
	connect(addKeyUI,
		SIGNAL(HideUISignal(luksaddkeyUI *)),
		this,
		SLOT(HideAddKeyUI(luksaddkeyUI *)));
	addKeyUI->partitionEntry(key);
}

void zuluCrypt::ShowAddKeyUI()
{
	luksaddkeyUI *addKeyUI = new luksaddkeyUI(this);
	addKeyUI->setWindowFlags(Qt::Window | Qt::Dialog);
	addKeyUI->setFont(this->font());
	connect(addKeyUI,
		SIGNAL(HideUISignal(luksaddkeyUI *)),
		this,
		SLOT(HideAddKeyUI(luksaddkeyUI *)));
	addKeyUI->ShowUI();
}

void zuluCrypt::ShowDeleteKeyContextMenu(QString key)
{
	luksdeletekey *deleteKeyUI = new luksdeletekey(this);
	deleteKeyUI->setWindowFlags(Qt::Window | Qt::Dialog);
	deleteKeyUI->setFont(this->font());
	connect(deleteKeyUI,
		SIGNAL(HideUISignal(luksdeletekey *)),
		this,
		SLOT(HideDeleteKeyUI(luksdeletekey *)));
	deleteKeyUI->deleteKey( key );
}

void zuluCrypt::ShowDeleteKeyUI()
{
	luksdeletekey *deleteKeyUI = new luksdeletekey(this);
	deleteKeyUI->setWindowFlags(Qt::Window | Qt::Dialog);
	deleteKeyUI->setFont(this->font());
	connect(deleteKeyUI,
		SIGNAL(HideUISignal(luksdeletekey *)),
		this,
		SLOT(HideDeleteKeyUI(luksdeletekey *)));
	deleteKeyUI->ShowUI();
}

void zuluCrypt::ShowCreateKeyFileUI()
{
	createkeyfile *createkeyFile = new createkeyfile(this);
	createkeyFile->setWindowFlags(Qt::Window | Qt::Dialog);
	createkeyFile->setFont(this->font());
	connect(createkeyFile,
		SIGNAL(HideUISignal(createkeyfile *)),
		this,
		SLOT(HideCreateKeyFileUI(createkeyfile *)));
	createkeyFile->ShowUI();
}

void zuluCrypt::showManageFavoritesUI()
{
	managedevicenames *managedevicenamesUI = new managedevicenames(this);
	managedevicenamesUI->setWindowFlags(Qt::Window | Qt::Dialog);
	managedevicenamesUI->setFont(this->font());
	connect(managedevicenamesUI,
		SIGNAL(HideUISignal(managedevicenames *)),
		this,
		SLOT(HideManageFavoritesUI(managedevicenames *)));
	managedevicenamesUI->ShowUI();
}

void zuluCrypt::ShowCreateFileUI()
{
	createfile *createFile = new createfile(this);
	createFile->setWindowFlags(Qt::Window | Qt::Dialog);
	createFile->setFont(this->font());

	connect(createFile,
		SIGNAL(HideUISignal(createfile *)),
		this,
		SLOT(HideCreateFileUI(createfile *)));
	connect(createFile,
		SIGNAL(fileCreated(QString)),
		this,
		SLOT(FileCreated(QString)));
	createFile->showUI();
}

void zuluCrypt::FileCreated(QString file)
{
	createpartition *createpartitionUI = new createpartition(this);
	createpartitionUI->setWindowFlags(Qt::Window | Qt::Dialog);
	createpartitionUI->setFont(this->font());

	connect(createpartitionUI,
		SIGNAL(HideUISignal(createpartition *)),
		this,
		SLOT(HideCreatePartitionUI(createpartition *)));

	createpartitionUI->ShowFileUI(file);
}

void zuluCrypt::ShowNonSystemPartitions()
{
	openpartition *NonSystemPartitions = new openpartition(this);
	NonSystemPartitions->setWindowFlags(Qt::Window | Qt::Dialog);
	NonSystemPartitions->setFont(this->font());

	connect(NonSystemPartitions,
		SIGNAL(HideUISignal(openpartition *)),
		this,
		SLOT(HideNonSystemPartitionUI(openpartition*)));

	createpartition *createpartitionUI = new createpartition(this);
	createpartitionUI->setWindowFlags(Qt::Window | Qt::Dialog);
	createpartitionUI->setFont(this->font());

	connect(createpartitionUI,
		SIGNAL(HideUISignal(createpartition *)),
		this,
		SLOT(HideCreatePartitionUI(createpartition *)));

	connect(NonSystemPartitions,
		SIGNAL(clickedPartition(QString)),
		createpartitionUI,
		SLOT(ShowPartitionUI(QString)));

	NonSystemPartitions->ShowNonSystemPartitions();
}

void zuluCrypt::volumeOpened(QString dev,QString m_point,password_Dialog *pd)
{
	addItemToTable(dev,m_point);
	pd->hide();
	pd->deleteLater();
}

password_Dialog * zuluCrypt::setUpPasswordDialog()
{
	password_Dialog *passwordDialogUI  = new password_Dialog(this) ;
	passwordDialogUI->setWindowFlags(Qt::Window | Qt::Dialog);
	passwordDialogUI->setFont(this->font());

	connect(passwordDialogUI,
		SIGNAL(HideUISignal(password_Dialog *)),
		this,
		SLOT(HidePasswordDialogUI(password_Dialog *)));

	connect(passwordDialogUI,
		SIGNAL(volumeOpened(QString,QString,password_Dialog *)),
		this,
		SLOT(volumeOpened(QString,QString,password_Dialog *))) ;

	connect(passwordDialogUI,
		SIGNAL(addItemToTable(QString,QString)),
		this,
		SLOT(addItemToTable(QString,QString))) ;

	return passwordDialogUI ;
}

void zuluCrypt::ShowPasswordDialogUI()
{
	setUpPasswordDialog()->ShowUI();
}

void zuluCrypt::ShowPasswordDialogUIFromFavorite(QString x, QString y)
{
	setUpPasswordDialog()->ShowUI(x,y);
}

void zuluCrypt::partitionClicked(QString partition)
{
	setUpPasswordDialog()->clickedPartitionOption(partition);
}

void zuluCrypt::ShowOpenPartitionUI()
{
	openpartition *openPartitionUI = new openpartition(this);
	openPartitionUI->setWindowFlags(Qt::Window | Qt::Dialog);
	openPartitionUI->setFont(this->font());

	connect(openPartitionUI,
		SIGNAL(clickedPartition(QString)),
		this,
		SLOT(partitionClicked(QString)));

	connect(openPartitionUI,
		SIGNAL(HideUISignal(openpartition *)),
		this,
		SLOT(HideOpenPartitionUI(openpartition *)));
}

void zuluCrypt::HideOpenPartitionUI(openpartition *obj)
{
	obj->deleteLater();
}

void zuluCrypt::HidePasswordDialogUI(password_Dialog *obj)
{
	obj->deleteLater();
}

void zuluCrypt::HideAddKeyUI(luksaddkeyUI *obj)
{
	obj->deleteLater();
}

void zuluCrypt::HideDeleteKeyUI(luksdeletekey *obj)
{
	obj->deleteLater();
}

void zuluCrypt::HideCreateKeyFileUI(createkeyfile *obj)
{
	obj->deleteLater();
}

void zuluCrypt::HideManageFavoritesUI(managedevicenames * obj)
{
	obj->deleteLater();
}

void zuluCrypt::HideCreatePartitionUI(createpartition *obj)
{
	obj->deleteLater();
}

void zuluCrypt::HideCreateFileUI(createfile *obj)
{
	obj->deleteLater();
}

void zuluCrypt::HideNonSystemPartitionUI(openpartition * obj)
{
	obj->deleteLater();
}

zuluCrypt::~zuluCrypt()
{
	delete rca ;
	delete ui;
	delete trayIcon;
	delete vpt ;
}
