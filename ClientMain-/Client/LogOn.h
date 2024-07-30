#pragma once
#pragma execution_character_set("utf-8") 
#include <QMainWindow>
#include "ui_LogOn.h"
#include "Login.h"
QT_BEGIN_NAMESPACE
namespace Ui { class LogOnClass; };
QT_END_NAMESPACE

class LogOn : public QMainWindow
{
	Q_OBJECT

public:
	LogOn(QWidget *parent = nullptr);
	~LogOn();
	QString get_ip()const;
private:
	Ui::LogOnClass *ui;
	Login* login_;
	bool isLogon_;
signals:
	void send_logon_msg(QString logonMsg);
	void send_ok(QByteArray frendList);
	
public slots:
	void on_logon_clicked();
	void on_login_clicked();
	void recv_login_msg(QString loginMsg);
	void recv_server(int head, QVariantMap params);
};
