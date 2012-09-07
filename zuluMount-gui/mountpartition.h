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

#include "managepartitionthread.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/userfont.h"

namespace Ui {
class mountPartition;
}

class mountPartition : public QWidget
{
	Q_OBJECT
public:
	explicit mountPartition( QWidget * parent = 0 );
	void ShowUI( QString path,QString mode,QString label ) ;
	void HideUI( void ) ;
	~mountPartition();
signals:
	void hideUISignal( void ) ;
	void mounted( void ) ;
private slots:
	void stateChanged( int ) ;
	void pbMount( void ) ;
	void pbCancel( void ) ;
	void pbOpenMountPath( void ) ;
	void slotMountComplete( int,QString ) ;
private:
	void closeEvent( QCloseEvent * );
	void saveOptions( QString,bool ) ;
	QStringList readOptions( void ) ;
	Ui::mountPartition * m_ui;
	QString m_path ;
	QString m_mode ;
	QString m_label ;
};

#endif // MOUNTPARTITION_H
