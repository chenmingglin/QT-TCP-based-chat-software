#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Client.h"
#include "CSocket.h"
#include "LogOn.h"
#include "User.h"   
#include <qlist.h>
QT_BEGIN_NAMESPACE
namespace Ui { class ClientClass; };
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();
    LogOn* logon_;
    CSocket* csocket_;
    QList<User> json_array_to_list(const QJsonArray& jsonArray);
    
private:
    Ui::ClientClass* ui;
    QList<User>friendlist_;
    User user_;
    int friendId_;
protected:
    inline void showEvent(QShowEvent* event) override;
signals:
    void friend_list_ok();
    void send_client_msg(QString msg);
    void send_friend_chatmsg(QVariantMap);
public slots:
    void recv_server(int head, QVariantMap params);
    void recv_friend_list(QByteArray friendData);
    void show_friend_list();
    void friend_listItem_clicked(QListWidgetItem* item);
    void clicked_sendBtn();
    void friend_searchBtn();
    void show_friend_chatMsg(QVariantMap params);//��ʾ����������Ϣ
    void show_ContextMenu(const QPoint& pos);//�Ҽ���ʾ�˵�
    void remove_SelectedItems();//�Ƴ���ѡ��ĺ���
};
