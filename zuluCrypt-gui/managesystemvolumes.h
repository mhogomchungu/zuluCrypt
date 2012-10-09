#ifndef MANAGESYSTEMVOLUMES_H
#define MANAGESYSTEMVOLUMES_H

#include <QDialog>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTableWidgetItem>
#include <QStringList>
#include <QMessageBox>
#include <QList>
#include <QKeySequence>
#include <QAction>
#include <QMenu>
#include <QCursor>

#include "openpartition.h"
#include "dialogmsg.h"

namespace Ui {
class manageSystemVolumes;
}

class manageSystemVolumes : public QDialog
{
	Q_OBJECT
public:
	explicit manageSystemVolumes( QWidget * parent = 0 );
	~manageSystemVolumes();
	void ShowUI( void ) ;
	void HideUI( void ) ;
signals:
	void HideUISignal( void );
private slots:
	void pbDone( void ) ;
	void pbFile( void ) ;
	void pbPartition( void ) ;
	void clickedPartition( QString ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous );
	void itemClicked( QTableWidgetItem * current,bool );
	void itemClicked( QTableWidgetItem * current );
	void removeCurrentRow( void ) ;
	void contextMenu( void ) ;
	void setFocusTableWidget( void );
private:
	void readSystemPartitions( void ) ;
	void writeSystemPartitions( void ) ;

	void addItemsToTable( QStringList );
	void addItemsToTable( QString );
	void closeEvent( QCloseEvent * ) ;
	Ui::manageSystemVolumes * m_ui;
	QAction * m_ac ;
};

#endif // MANAGESYSTEMVOLUMES_H
