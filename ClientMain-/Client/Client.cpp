#include "Client.h"
#include <qpixmap.h>
#include <qbuffer.h>
#include <qlistwidget.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlistwidget.h>
#include <qfile.h>
#include <qdir.h>
Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientClass())
    , logon_(new LogOn(this))
    , csocket_(new CSocket(this))
{
    logon_->show();
    csocket_->socket_->connectToHost(QHostAddress(logon_->get_ip()), 8989);
    connect(logon_, &LogOn::send_logon_msg, csocket_, &CSocket::recv_msg);
    connect(csocket_, &CSocket::send_to_logon, logon_, &LogOn::recv_server);
    connect(logon_, &LogOn::send_ok, this, &Client::recv_friend_list);
    connect(csocket_, &CSocket::send_to_client, this, &Client::recv_server);

    connect(this, &Client::friend_list_ok, this, &Client::show_friend_list);
    
    ui->setupUi(this);
}

Client::~Client()
{
    delete ui;
}

QList<User> Client::json_array_to_list(const QJsonArray& jsonArray)
{
    QList<User> userList;
    for (const QJsonValue& value : jsonArray) {
        if (value.isObject()) {
            QJsonObject jsonObject = value.toObject();
            if (jsonObject.contains("friendId") && jsonObject.contains("friendName")) {
                int userId = jsonObject["friendId"].toInt();
                QString userName = jsonObject["friendName"].toString();
                User user(userId, userName);
                userList.append(user);
            }
        }
    }
    return userList;
}

void Client::recv_friend_list(QByteArray friendData)
{
    QJsonParseError jsonError;
    //QBytearray转换为json
    QJsonDocument doc = QJsonDocument::fromJson(friendData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        if (doc.isArray())
        {
            QJsonArray jsonArray = doc.array();
            qDebug() << "JSON Array parsed successfully.123";
            qDebug() << jsonArray;
            friendlist_ = json_array_to_list(jsonArray);
            emit friend_list_ok();
        }
    }
    else
    {
        qDebug() << "Error parsing JSON data:" << jsonError.errorString();

    }
}

void Client::show_friend_list()
{
    for (int i = 0; i < friendlist_.length(); i++)
    {
        QListWidgetItem* newItem = new QListWidgetItem(friendlist_.at(i).name() + ": " + QString::number(friendlist_.at(i).id()));
        newItem->setData(32, friendlist_.at(i).id());
        newItem->setSizeHint(QSize(240, 65));
        newItem->setIcon(QIcon(":/img/1.jpeg"));
        ui->listWidget->addItem(newItem);
    }
}

//接受服务器发来的信息
void Client::recv_server(int head, QVariantMap params)
{
    switch (head)
    {
    case 2://好友列表

    default:
        break;
    }
}
