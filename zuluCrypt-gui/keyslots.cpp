#include "keyslots.h"
#include "ui_keyslots.h"

keyslots::keyslots(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::keyslots)
{
	ui->setupUi(this);
}

keyslots::~keyslots()
{
	delete ui;
}
