#pragma once
#pragma execution_character_set("utf-8") 

#include <QtWidgets/QMainWindow>
#include "ui_ClientMain.h"
#include <qtcpsocket.h>
#include "login.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ClientMainClass; };
QT_END_NAMESPACE

class ClientMain : public QMainWindow
{
    Q_OBJECT

public:
    ClientMain(QWidget *parent = nullptr);
    ~ClientMain();
    QString getIp() const;

signals:
    void sendData(int operation, QVariantMap params);
    void loginClose();
private:
    Ui::ClientMainClass *ui;
    QTcpSocket* m_socket;

public slots:
    void onClickedLogon();
    void onClickedLogin();
    void recvSocketData();
    void recvData(int operation, QVariantMap params);

};
