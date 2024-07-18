#include "MsgMain.h"
#include <qpixmap.h>
#include <qbuffer.h>
#include <qlistwidget.h>
MsgMain::MsgMain(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MsgMainClass())
{
	ui->setupUi(this);
	ui->widget->setStyleSheet("background-color: black;");
	ui->icon->setStyleSheet("background-image: url(:/icon.jpg);");
	QListWidgetItem* newItem = new QListWidgetItem("ÕÅÈý");
	newItem->setSizeHint(QSize(250, 65));
	ui->listWidget->addItem(newItem);

}

MsgMain::~MsgMain()
{
	delete ui;
}


