#include "Login.h"
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qvariantmap.h>

Login::Login(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::LoginClass())
{
	ui->setupUi(this);
	connect(ui->ok, &QPushButton::clicked, this, &Login::on_login_ok);
}

Login::~Login()
{
	delete ui;
}

void Login::on_login_ok()
{
    QString name = ui->name->text();
    QString psd = ui->psd->text();

    qDebug() << "name :" << name;
    qDebug() << "psd :" << psd;
    QJsonObject obj;
    obj.insert("head", 3);
    obj.insert("name", name);
    obj.insert("psd", psd);
    obj.insert("msg", "зЂВс");
    obj.insert("userId", 0);
    QJsonDocument doc(obj);
    QByteArray loginMsg = doc.toJson();

    emit send_login_msg(loginMsg);
}