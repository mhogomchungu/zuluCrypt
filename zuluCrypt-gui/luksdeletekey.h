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

#ifndef LUKSDELETEKEY_H
#define LUKSDELETEKEY_H

#include <QDialog>
#include <QString>
#include <QStringList>

#include "utility.h"

class QWidget ;

namespace Ui {
class luksdeletekey ;
}

class luksdeletekey : public QDialog
{
	Q_OBJECT
public:
	explicit luksdeletekey( QWidget *parent = 0 ) ;
	~luksdeletekey() ;
signals:
	void pbDeleteClicked( QString volumePath,bool passPhraseIsFile,QString passPhrase ) ;
	void pbOpenPartitionClicked( void ) ;
	void HideUISignal( void ) ;
public slots:
	void ShowUI( void ) ;
	void ShowUI( const QString& ) ;
	void HideUI( void ) ;
private slots:
	void pbOpenPartition( void ) ;
	void pbDelete( void ) ;
	void pbCancel( void ) ;
	void rbPassphrase( void ) ;
	void rbPassphraseFromFile( void ) ;
	void pbOpenKeyFile( void ) ;
	void pbOpenVolume( void ) ;
	void taskFinished( int ) ;
	void deleteKey( const QStringList& ) ;
private:
	void disableAll( void ) ;
	void enableAll( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::luksdeletekey * m_ui ;
	QString m_volumePath ;
	bool m_isWindowClosable ;
	int m_keyNumber ;
	QString m_totalKeys ;
};

#endif // LUKSDELETEKEY_H
