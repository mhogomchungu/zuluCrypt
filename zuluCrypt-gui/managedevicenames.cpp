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

#include "managedevicenames.h"
#include "ui_managedevicenames.h"
#include <iostream>

managedevicenames::managedevicenames(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::managedevicenames)
{
	ui->setupUi(this);
	connect(ui->pbDeviceAddress,
		SIGNAL(clicked()),
		this,
		SLOT(DeviceAddress()));
	connect(ui->pbAdd,
		SIGNAL(clicked()),
		this,
		SLOT(Add()));
	connect(ui->pbFileAddress,
		SIGNAL(clicked()),
		this,
		SLOT(FileAddress()));
	connect(ui->pbCancel,
		SIGNAL(clicked()),
		this,
		SLOT(Cancel()));
	connect(ui->tableWidget,
		SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
		this,
		SLOT(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)));
	connect(ui->tableWidget,
		SIGNAL(itemClicked(QTableWidgetItem*)),
		this,
		SLOT(itemClicked(QTableWidgetItem*)));
	connect(ui->lineEditDeviceAddress,
		SIGNAL(textChanged(QString)),
		this,
		SLOT(devicePathTextChange(QString)));

	ui->pbFileAddress->setIcon(QIcon(QString(":/keyfile.png")));
	ui->pbDeviceAddress->setIcon(QIcon(QString(":/partition.png")));
	ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter );
	keys.append( Qt::Key_Return );
	ac->setShortcuts(keys) ;
	connect(ac,SIGNAL(triggered()),this,SLOT(shortcutPressed())) ;
	this->addAction( ac );
}

void managedevicenames::devicePathTextChange(QString txt)
{
	if(txt == QString("")){
		ui->lineEditMountPath->setText( "" ) ;
		return ;		
	}
	QString ed = QDir::homePath() + QString("/") + txt.split("/").last() ;
	ui->lineEditMountPath->setText( ed );
}

void managedevicenames::shortcutPressed()
{
	QTableWidgetItem *it = ui->tableWidget->currentItem() ;
	if( it == NULL )
		return ;
	itemClicked( it );
}
void managedevicenames::deletePartitionUI()
{
	openPartitionUI->deleteLater();
}

void managedevicenames::ShowUI()
{
	openPartitionUI = new openpartition(this) ;
	openPartitionUI->setWindowFlags(Qt::Window | Qt::Dialog);

	ui->tableWidget->setColumnWidth(0,296);
	ui->tableWidget->setColumnWidth(1,296);

	connect(this,
		SIGNAL(ShowPartitionUI()),
		openPartitionUI,
		SLOT(ShowUI()));
	connect(openPartitionUI,
		SIGNAL(clickedPartition(QString)),
		this,
		SLOT(PartitionEntry(QString)));
	connect(openPartitionUI,
		SIGNAL(HideUISignal()),
		this,
		SLOT(deletePartitionUI()));

	this->show();

	while( ui->tableWidget->rowCount() > 0)
		ui->tableWidget->removeRow(0);

	QStringList entries = miscfunctions::readFavorites() ;
	QStringList line ;
	for( int i = 0 ; i < entries.size() - 1 ; i++){
		line = entries.at(i).split("\t");
		addEntries(line.at(0),line.at(1));
	}
	ui->lineEditDeviceAddress->clear();
	ui->lineEditMountPath->clear();
	ui->tableWidget->setFocus();
}

void managedevicenames::HideUI()
{
	this->hide();	
}

void managedevicenames::addEntries(QString dev, QString m_point)
{
	QTableWidgetItem *item = new QTableWidgetItem() ;
	int row = ui->tableWidget->rowCount() ;
	ui->tableWidget->insertRow(row);
	item->setText( dev );
	item->setTextAlignment(Qt::AlignCenter);
	ui->tableWidget->setItem(row,0,item);
	item = new QTableWidgetItem() ;
	item->setText( m_point );
	item->setTextAlignment(Qt::AlignCenter);
	ui->tableWidget->setItem(row,1,item);
	ui->tableWidget->setCurrentCell(row ,1);
}

void managedevicenames::itemClicked(QTableWidgetItem *current)
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowTitle(QString("WARNING"));
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText("delete this entry from the favorite list?");
	m.addButton(QMessageBox::Yes);
	m.addButton(QMessageBox::No);
	if( m.exec() == QMessageBox::No )
		return ;
	QString entry = ui->tableWidget->item(current->row(),0)->text() + \
			QString("\t") + \
			ui->tableWidget->item(current->row(),1)->text() + \
			QString("\n") ;
	miscfunctions::removeFavoriteEntry(entry);
	ui->tableWidget->removeRow(current->row());
	HighlightRow(0,-1);
}

void managedevicenames::Cancel()
{
	HideUI();
	emit HideUISignal(this);
}

void managedevicenames::Add()
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowTitle(QString("ERROR"));
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	QString dev = ui->lineEditDeviceAddress->text() ;
	QString m_point = ui->lineEditMountPath->text() ;
	
	if( dev == QString("")){
		m.setText("device address field is empty");
		m.addButton(QMessageBox::Ok);
		m.exec();
		return ;
	}
	if( m_point == QString("")){
		m.setText("mount point path field is empty");
		m.addButton(QMessageBox::Ok);
		m.exec();
		return ;
	}
	addEntries(dev,m_point);
	miscfunctions::addToFavorite(dev,m_point);
	
	ui->lineEditDeviceAddress->clear(); ;
	ui->lineEditMountPath->clear() ;
}

void managedevicenames::DeviceAddress()
{
	emit ShowPartitionUI();
}

void managedevicenames::FileAddress()
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("path to an encrypted file"),
						 QDir::homePath(),
						 0);
	ui->lineEditDeviceAddress->setText( Z ) ;
}

void managedevicenames::PartitionEntry(QString device)
{
	ui->lineEditDeviceAddress->setText( device );
	//ui->lineEditMountPath->setFocus();
}

managedevicenames::~managedevicenames()
{
	delete ui;
	delete ac;
}

void managedevicenames::closeEvent(QCloseEvent *e)
{
	e->ignore();
	HideUI();
}

void managedevicenames::currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous )
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

void managedevicenames::HighlightRow(int currentRow,int previousRow)
{
	int count = ui->tableWidget->rowCount() ;
	if(count > 0 && currentRow != -1){
		ui->tableWidget->item(currentRow,0)->setSelected(true);
		ui->tableWidget->item(currentRow,1)->setSelected(true);
		ui->tableWidget->setCurrentItem(ui->tableWidget->item(currentRow,1));
	}
	if(count > 1 && currentRow != previousRow && previousRow != -1 ){
		ui->tableWidget->item(previousRow,0)->setSelected(false);
		ui->tableWidget->item(previousRow,1)->setSelected(false);
	}
}
