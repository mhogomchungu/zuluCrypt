#ifndef KWALLETCONFIG_H
#define KWALLETCONFIG_H

#include <QWidget>
#include <QCloseEvent>

#include "kwalletplugin.h"
#include "dialogmsg.h"
#include "openpartition.h"

#include <Qt>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMap>
#include <QFileDialog>

namespace Ui {
class kwalletconfig;
}

class kwalletconfig : public QWidget
{
	Q_OBJECT

public:
	explicit kwalletconfig( QWidget * parent = 0 );
	~kwalletconfig();
	void ShowUI( void ) ;
	void HideUI( void ) ;
signals:
	void HideUISignal( void ) ;

private slots:
	void pbGetUUIDFromFile( void ) ;
	void selectedVolume( QString ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous );
	void pbDelete( void ) ;
	void pbAdd( void ) ;
	void pbClose( void ) ;
	void pbGetUUIDFromDevices( void ) ;
private:
	void HighlightRow( int,bool ) ;
	void ShowWalletEntries( void );
	Ui::kwalletconfig * m_ui;
	void closeEvent( QCloseEvent * ) ;
	kwalletplugin * m_wallet ;
	QMap < QString,QString > m_map ;
};

#endif // KWALLETCONFIG_H
