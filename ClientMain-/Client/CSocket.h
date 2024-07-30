#pragma once

#include <QObject>
#include <qtcpsocket.h>
class CSocket : public QObject
{
	Q_OBJECT

public:
	CSocket(QObject *parent);
	~CSocket();
	QTcpSocket* socket_;

signals:
	void send_to_logon(int head, QVariantMap params);
	void send_to_client(int head, QVariantMap params);
public slots:
	void recv_server_data();
	void recv_msg(QString);
};
