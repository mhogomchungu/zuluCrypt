/*
 *
 *  Copyright ( c ) 2011-2015
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

#ifndef OPEN_PARTITION_H
#define OPEN_PARTITION_H

#include<QString>
#include<QStringList>
#include<QDialog>

#include "utility.h"

class QWidget ;
class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;

namespace Ui {
class openvolume ;
}

class openvolume :  public QDialog
{
	Q_OBJECT
public:
	static openvolume& instance( QWidget * parent,bool e )
	{
		return *( new openvolume( parent,e ) ) ;
	}
	openvolume( QWidget * parent,bool ) ;
	virtual ~openvolume() ;
	openvolume& showEncryptedOnly( void ) ;
	openvolume& showLuksOnly( void ) ;
	openvolume& setOnExit( std::function< void() > ) ;
	void ShowAllPartitions( std::function< void( const QString& ) > ) ;
	void ShowNonSystemPartitions( std::function< void( const QString& ) > ) ;
	void partitionList( const QString&,const QString&,std::function< void( const QString& ) > ) ;
public slots:
	void tableEntryDoubleClicked( QTableWidgetItem * ) ;
	void HideUI( void ) ;
	void allowLUKSOnly( void ) ;
private slots:
	void pbOpen( void ) ;
	void pbHelp( void ) ;
	void pbUUID( void ) ;
	void pbCancel( void ) ;
	void EnterKeyPressed( void ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
private:
	void disableAll( void ) ;
	void enableAll( void ) ;
	void partitionList( const QString&,const QString& ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::openvolume * m_ui ;
	enum { nonSystemVolumes,systemVolumes,allVolumes } m_option ;
	bool m_disableNonLUKS = false ;
	bool m_showEncryptedOnly = false ;
	bool m_showLuksOnly = false ;
	bool m_enableUUID ;
	std::function< void( const QString& ) > m_function ;
	std::function< void() > m_onExit = [](){} ;
};

#endif
