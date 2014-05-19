
#include "tablewidget.h"
#include <QDebug>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>

void tablewidget::selectTableRow( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	QTableWidget * table ;

	int col = 0 ;
	int i   = 0 ;
	int j   = 0 ;

	if( current && previous ){

		if( previous->row() == current->row() ){
			table = current->tableWidget() ;
			table->setCurrentCell( current->row(),table->columnCount() - 1 ) ;
			table->setFocus() ;
			return ;
		}
	}

	if( current ){

		table = current->tableWidget() ;

		if( table->rowCount() > 0 ){

			col = table->columnCount() ;
			j = current->row() ;

			for( i = 0 ; i < col ; i++ ){
				table->item( j,i )->setSelected( true ) ;
			}
		}

		table->setCurrentCell( j,table->columnCount() -1 ) ;
		table->setFocus() ;
	}

	if( previous ){

		table = previous->tableWidget() ;

		if( table->rowCount() > 0 ){

			col = table->columnCount() ;
			j = previous->row() ;

			for( i = 0 ; i < col ; i++ ){
				table->item( j,i )->setSelected( false ) ;
			}
		}

		table->setFocus() ;
	}
}

int tablewidget::addEmptyRow( QTableWidget * table )
{
	QTableWidgetItem * item ;
	int row = table->rowCount() ;
	int col = table->columnCount() ;
	table->insertRow( row ) ;
	for( int i = 0 ; i < col ; i++ ){
		item = new QTableWidgetItem() ;
		item->setTextAlignment( Qt::AlignCenter ) ;
		table->setItem( row,i,item ) ;
	}
	table->setCurrentCell( row,col - 1 ) ;
	return row ;
}

int tablewidget::columnHasEntry( QTableWidget * table,const QString& entry,int column )
{
	int rows = table->rowCount() ;
	for( int i = 0 ; i < rows ; i++ ){
		if( table->item( i,column )->text() == entry ){
			return i ;
		}
	}
	return -1 ;
}

void tablewidget::addRowToTable( QTableWidget * table,const QStringList& list )
{
	QTableWidgetItem * item ;

	int j = list.size() ;

	if( j != table->columnCount() ){
		qDebug() << "ERROR: table column count is NOT the same as QStringList size ";
		return ;
	}

	int row = table->rowCount() ;

	table->insertRow( row ) ;

	for( int i = 0 ; i < j ; i++ ){

		item = new QTableWidgetItem() ;
		item->setText( list.at( i ) ) ;
		item->setTextAlignment( Qt::AlignCenter ) ;
		table->setItem( row,i,item ) ;
	}

	table->setCurrentCell( row,j - 1 ) ;
}

void tablewidget::addRowToTable( QTableWidget * table,const QStringList& list,const QFont& font )
{
	QTableWidgetItem * item ;

	int j = list.size() ;

	if( j != table->columnCount() ){
		qDebug() << "ERROR: table column count is NOT the same as QStringList size ";
		return ;
	}

	int row = table->rowCount() ;

	table->insertRow( row ) ;

	for( int i = 0 ; i < j ; i++ ){

		item = new QTableWidgetItem() ;
		item->setText( list.at( i ) ) ;
		item->setTextAlignment( Qt::AlignCenter ) ;
		item->setFont( font ) ;
		table->setItem( row,i,item ) ;
	}

	table->setCurrentCell( row,j - 1 ) ;
}

void tablewidget::setRowFont( QTableWidget * table ,int row,const QFont& font )
{
	if( row < table->rowCount() ){
		int j = table->columnCount() ;
		for( int i = 0 ; i < j ; i++ ){
			table->item( row,i )->setFont( font ) ;
		}
	}
}

void tablewidget::deleteRowFromTable( QTableWidget * table,int row )
{
	if( row < table->rowCount() ){
		table->removeRow( row ) ;
		if( table->rowCount() > 0 ){
			table->setCurrentCell( table->rowCount() - 1,table->columnCount() - 1 ) ;
		}
	}
	table->setFocus() ;
}

void tablewidget::deleteRowFromTable( QTableWidget * table,const QString& value,int column )
{
	int j = table->rowCount() ;
	for( int row = 0 ; row < j ; row++ ){
		if( table->item( row,column )->text() == value ){
			tablewidget::deleteRowFromTable( table,row ) ;
		}
	}
}

void tablewidget::deleteTableRow( QTableWidget * table,const QString& value,int column )
{
	int j = table->rowCount() ;
	for( int row = 0 ; row < j ; row++ ){
		if( table->item( row,column )->text() == value ){
			table->removeRow( row ) ;
			break ;
		}
	}
}

void tablewidget::selectRow( QTableWidget * table,int row )
{
	if( row < table->rowCount() ){
		table->setCurrentCell( row,table->columnCount() - 1 ) ;
	}
}

void tablewidget::selectLastRow( QTableWidget * table )
{
	int row = table->rowCount() - 1 ;
	if( row >= 0 ){
		table->setCurrentCell( row,table->columnCount() - 1 ) ;
	}
	table->setFocus() ;
}

void tablewidget::setText( QTableWidget * table,int row,int col,const QString& text )
{
	QFont f = table->item( row,col )->font() ;

	QTableWidgetItem * item = new QTableWidgetItem() ;
	item->setText( text ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	item->setFont( f ) ;
	table->setItem( row,col,item ) ;
}

QStringList tablewidget::tableEntries( QTableWidget * table,int column )
{
	QStringList l ;
	int col = table->columnCount() ;
	if( column < col ){
		int j = table->rowCount() ;
		for( int row = 0 ; row < j ; row++ ){
			l.append( table->item( row,column )->text() ) ;
		}
	}
	return l ;
}
