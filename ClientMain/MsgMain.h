#pragma once

#include <QMainWindow>
#include "ui_MsgMain.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MsgMainClass; };
QT_END_NAMESPACE

class MsgMain : public QMainWindow
{
	Q_OBJECT

public:
	MsgMain(QWidget *parent = nullptr);
	~MsgMain();

private:
	Ui::MsgMainClass *ui;
};
