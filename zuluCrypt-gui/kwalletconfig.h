#ifndef KWALLETCONFIG_H
#define KWALLETCONFIG_H

#include <QWidget>
#include <QCloseEvent>

#include "kwalletplugin.h"

#include <Qt>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace Ui {
class kwalletconfig;
}

class kwalletconfig : public QWidget
{
	Q_OBJECT

public:
	explicit kwalletconfig( QWidget * parent = 0);
	~kwalletconfig();
	void ShowUI( void ) ;
	void HideUI( void ) ;
signals:
	void HideUISignal( void ) ;

private slots:
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous );
	void pbDelete( void ) ;
	void pbAdd( void ) ;
	void pbClose( void ) ;
	void pbShowDevices( void ) ;
private:
	void HighlightRow( int,bool ) ;
	void ShowWalletEntries( void );
	Ui::kwalletconfig * m_ui;
	void closeEvent( QCloseEvent * ) ;
	kwalletplugin * m_wallet ;
};

#endif // KWALLETCONFIG_H
