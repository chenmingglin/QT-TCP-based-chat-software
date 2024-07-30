#pragma once

#include <QMainWindow>
#include "ui_Login.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginClass; };
QT_END_NAMESPACE

class Login : public QMainWindow
{
	Q_OBJECT

public:
	Login(QWidget *parent = nullptr);
	~Login();
	Ui::LoginClass* ui;
private:
	

signals:
	void send_login_msg(QString loginMsg);

public:
	void on_login_ok();
};
