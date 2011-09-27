#ifndef CREATEPARTITION_H
#define CREATEPARTITION_H

#include <QWidget>
#include <QString>

namespace Ui {
    class createpartition;
}

class createpartition : public QWidget
{
	Q_OBJECT

public:
	explicit createpartition(QWidget *parent = 0);
	~createpartition();

signals:
	void CreateVolume(QString fileSystem,QString containterType,QString volumePath,QString passphrase,bool passphraseFromFile) ;

public slots:
	void ShowPartitionUI(QString volume) ;
	void ShowFileUI(QString volume) ;

private slots:
	void pbCreateClicked(void) ;
	void pbCancelClicked(void) ;
	void rbPassphraseClicked(void);
	void rbPasssphraseFromFileClicked(void);
	void pbOpenKeyFile(void);

private:
	Ui::createpartition *ui;
};

#endif // CREATEPARTITION_H
