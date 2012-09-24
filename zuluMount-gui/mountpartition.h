#ifndef MOUNTPARTITION_H
#define MOUNTPARTITION_H

#include <QWidget>
#include <QFile>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QCloseEvent>
#include <QByteArray>
#include <QFileDialog>
#include <QFont>
#include <QTableWidget>
#include "managepartitionthread.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/userfont.h"
#include "../zuluCrypt-gui/openvolumereadonly.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "mainwindow.h"

namespace Ui {
class mountPartition;
}

class mountPartition : public QWidget
{
	Q_OBJECT
public:
	explicit mountPartition( QWidget * parent = 0,QTableWidget * table = 0 );
	void ShowUI( QString path,QString label ) ;
	void HideUI( void ) ;
	~mountPartition();
signals:
	void hideUISignal( void ) ;
	void mounted( QString ) ;
private slots:
	void stateChanged( int ) ;
	void pbMount( void ) ;
	void pbCancel( void ) ;
	void pbOpenMountPath( void ) ;
	void slotMountComplete( int,QString ) ;
	void checkBoxReadOnlyStateChanged( int ) ;
	void volumeMiniProperties( QString ) ;
private:
	void closeEvent( QCloseEvent * );
	void saveOptions( QString,bool ) ;
	QStringList readOptions( void ) ;
	Ui::mountPartition * m_ui;
	QString m_path ;
	QString m_label ;
	QTableWidget * m_table ;
};

#endif // MOUNTPARTITION_H
