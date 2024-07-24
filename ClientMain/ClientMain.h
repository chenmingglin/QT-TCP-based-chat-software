#pragma once
#pragma execution_character_set("utf-8") 

#include <QtWidgets/QMainWindow>
#include "ui_ClientMain.h"
#include <qtcpsocket.h>
#include "login.h"
#include "MsgMain.h"
#include "User.h"


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
    void sendFriendsList(QByteArray friendsData);
    void sendUserId(int userId);
    void sendFriendMsg(QVariantMap params);
private:
    Ui::ClientMainClass *ui;
    QTcpSocket* m_socket;
    MsgMain* m_msgWidget;
    
public slots:
    void onClickedLogon();
    void onClickedLogin();
    void recvSocketData();
    void recvData(int operation, QVariantMap params);
 

    void recvUserMsgtoFriend(QByteArray msg);
};
