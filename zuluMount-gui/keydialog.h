#ifndef KEYDIALOG_H
#define KEYDIALOG_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDir>
#include <QMenu>
#include <QTableWidget>

#include "../zuluCrypt-gui/dialogmsg.h"
#include "managepartitionthread.h"
#include "../zuluCrypt-cli/constants.h"
#include "plugin_path.h"
#include "../zuluCrypt-gui/zuluoptions.h"
#include "../zuluCrypt-gui/socketsendkey.h"
#include "../zuluCrypt-gui/openvolumereadonly.h"
#include "../zuluCrypt-gui/openmountpointinfilemanager.h"
#include "../zuluCrypt-gui/savemountpointpath.h"

#include "mainwindow.h"

namespace Ui {
class keyDialog;
}

class keyDialog : public QDialog
{
	Q_OBJECT
public:
	keyDialog( QWidget * parent,QTableWidget *,QString path,QString type );
	~keyDialog();
	void ShowUI( void );
	void HideUI( void ) ;
signals:
	void mounted( QString ) ;
	void cryptoOpen( QString ) ;
	void hideUISignal( void ) ;
public slots:
	void slotMountComplete( int,QString ) ;
private slots:
	void pbkeyOption( void ) ;
	void pbMountPointPath( void ) ;
	void pbPluginEntryClicked( QAction * ) ;
	void keyTextChanged( QString ) ;
	void rbPlugIn( bool ) ;
	void rbKey( bool ) ;
	void rbKeyFile( bool ) ;
	void pbOpen( void ) ;
	void pbCancel( void ) ;
	void Plugin( void );
	void KeyFile( void ) ;
	void cbMountReadOnlyStateChanged( int ) ;
	void volumeMiniProperties( QString ) ;
private:
	void enableAll( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * );
	Ui::keyDialog * m_ui;
	QString m_path ;
	QMenu * m_menu ;
	QTableWidget * m_table ;
	bool m_working ;
};

#endif // KEYDIALOG_H
