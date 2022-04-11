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

#ifndef luksaddkeySUI_H
#define luksaddkeySUI_H

#include <QDialog>
#include <QString>
#include <QStringList>

#include "utility.h"
#include "keystrength.h"

namespace Ui {
class luksaddkey ;
}

class QWidget ;
class QCloseEvent ;
class keystrength ;

class luksaddkey : public QDialog
{
	Q_OBJECT
public:
	static luksaddkey& instance( QWidget * parent )
	{
		return *( new luksaddkey( parent ) ) ;
	}
	explicit luksaddkey( QWidget * parent = 0 ) ;
	~luksaddkey() ;
signals :
	void clickedpbAdd( QString PathToVolume,bool keyfile,QString ExistingKey,bool newkeyfile,QString NewKey ) ;
	void pbOpenPartitionClicked( void ) ;
public slots:
	void ShowUI( const QString& ) ;
	void ShowUI( const QString&,const QString&,std::function< void() > ) ;
	void ShowUI( void ) ;
	void HideUI( void ) ;
private slots:
	void cbVolumeType( int ) ;
	void cbExistingKey( int ) ;
	void cbNewKey( int ) ;
	void keyChanged_0( QString ) ;
	void keyChanged_1( QString ) ;
	void pbOpenExisitingKeyFile( void ) ;
	void pbOpenNewKeyFile( void ) ;
	void pbOpenFile( void ) ;
	void pbOpenPartition( void ) ;
	void pbAdd( void ) ;
	void pbCancel( void ) ;
	void taskFinished( const utility::Task& ) ;
	void setFieldFocus( void ) ;
private:
	void keyAdded( void ) ;
	void disableAll( void ) ;
	void enableAll( void ) ;
	void luks2Cancel() ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::luksaddkey * m_ui ;
	QString m_volumePath ;
	bool m_isWindowClosable ;
	bool m_yubikeyExistingKey ;
	bool m_yubekeyNewKey ;
	bool m_showingLuks2AdvanceOptions = false ;
	keystrength m_keystrength ;
	utility::veraCryptWarning m_veraCryptWarning ;
	utility::label m_label ;
	std::function< void() > m_onExit = [](){} ;
};

#endif // luksaddkeySUI_H
