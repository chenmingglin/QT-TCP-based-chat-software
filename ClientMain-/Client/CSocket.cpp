#include "CSocket.h"
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qvariantmap.h>
CSocket::CSocket(QObject *parent)
	: QObject(parent), socket_(new QTcpSocket())
{
    connect(socket_, &QTcpSocket::readyRead, this, &CSocket::recv_server_data);
}

CSocket::~CSocket()
{
    socket_->deleteLater();
}

void CSocket::recv_server_data()
{
    QByteArray data = socket_->readAll();
    QJsonDocument doc = doc.fromJson(data);
    QJsonObject obj = doc.object();

    QVariantMap params = obj.toVariantMap();
    int head = params.value("head").toInt();
    switch (head)
    {
    case 1://发给登录界面--登录
        emit send_to_logon(1, params);
        break;

    case 2://发给主界面--好友信息
        emit send_to_client(2, params);
        break;
    case 3://发给登录界面--注册
        emit send_to_logon(3, params);
        break;
    case 4://发给主界面--添加好友
        emit send_to_client(4, params);
        break;
    default:
        break;
    }
    socket_->flush();
}

void CSocket::recv_msg(QString msg)
{
    socket_->write(msg.toUtf8());
}