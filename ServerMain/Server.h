#pragma once

#include <QTcpServer>
#include <qtcpsocket.h>
#include <qmap.h>
#include "Sql.h"
#include <qvariantmap.h>
#include <qjsonarray.h>
class Server : public QTcpServer
{
	Q_OBJECT

public:
	Server(QObject* parent = nullptr);
	~Server();

signals:
	void databaseOperationRequested(int operation, QVariantMap parameters, QTcpSocket* socket);

public slots:
	void readReady();
	void disconnected();
	//将好友list转换为json
	QJsonArray userListToJsonArray(const QList<User>& users);
	


private:
	QMap<int, QTcpSocket*> m_clientsockets;
	void incomingConnection(qintptr socketDesc) override;
	Sql* m_sql;
private slots:
	void handleDatabaseOperation(int operation, QVariantMap parameters, QTcpSocket* socket);
};
