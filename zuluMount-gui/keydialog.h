#ifndef KEYDIALOG_H
#define KEYDIALOG_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDir>
#include <QMenu>

#include "../zuluCrypt-gui/dialogmsg.h"
#include "managepartitionthread.h"
#include "../zuluCrypt-gui/miscfunctions.h"
#include "../zuluCrypt-cli/constants.h"
#include "plugin_path.h"

namespace Ui {
class keyDialog;
}

class keyDialog : public QDialog
{
	Q_OBJECT
public:
	keyDialog( QWidget * parent,QString path,QString mode );
	~keyDialog();
	void ShowUI( void );
	void HideUI( void ) ;
signals:
	void cryptoOpen( QString ) ;
	void hideUISignal( void ) ;
	void updatePartitions( void ) ;
public slots:
	void slotMountComplete( int,QString ) ;
private slots:
	void pbMountPointPath( void ) ;
	void pbPluginEntryClicked( QAction * ) ;
	void keyTextChanged( QString ) ;
	void rbPlugIn( bool ) ;
	void rbKey( bool ) ;
	void rbKeyFile( bool ) ;
	void pbOpen( void ) ;
	void pbCancel( void ) ;
	void pbPlugin( void );
private:
	void enableAll( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * );
	Ui::keyDialog * m_ui;
	QString m_path ;
	QString m_mode ;
	QMenu * m_menu ;
};

#endif // KEYDIALOG_H
