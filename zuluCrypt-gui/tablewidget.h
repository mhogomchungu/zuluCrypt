/*
 *
 *  Copyright ( c ) 2012-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QString>
#include <QStringList>
#include <QFont>

class QTableWidget ;
class QTableWidgetItem ;

class tablewidget
{
public:
	static void selectTableRow( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	static void addRowToTable( QTableWidget *,const QStringList& ) ;
	static void addRowToTable( QTableWidget *,const QStringList&,const QFont& ) ;
	static void updateRowInTable( QTableWidget *,const QStringList&,int row,const QFont& ) ;
	static void setRowFont( QTableWidget *,int row,const QFont& ) ;
	static void deleteRowFromTable( QTableWidget *,int row ) ;
	static void deleteRowFromTable( QTableWidget *,const QString&,int column = 0 ) ;
	static void deleteTableRow( QTableWidget *,const QString&,int column = 0 ) ;
	static void selectRow( QTableWidget *,int row ) ;
	static void selectRow( QTableWidget *,const QString& ) ;
	static void selectLastRow( QTableWidget * ) ;
	static void setText( QTableWidget *,int row,int col,const QString& text ) ;
	static int addEmptyRow( QTableWidget * ) ;
	static int columnHasEntry( QTableWidget *,const QString&,int column = 0 ) ;
	static QStringList tableColumnEntries( QTableWidget * table,int column = 0 ) ;
	static QStringList tableRowEntries( QTableWidget * table,int row = 0 ) ;
};

#endif // TABLEWIDGET_H
