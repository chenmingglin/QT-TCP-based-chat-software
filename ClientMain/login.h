#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_login.h"

QT_BEGIN_NAMESPACE
namespace Ui { class loginClass; };
QT_END_NAMESPACE

class Login : public QMainWindow
{
	Q_OBJECT

public:
	Login(QWidget *parent = nullptr);
	~Login();

	QString name()const;
	QString psd()const;
private:
	Ui::loginClass *ui;

signals:
	void onClieckedOk();
};
