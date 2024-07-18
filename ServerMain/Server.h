#pragma once

#include <QTcpServer>
#include <qtcpsocket.h>
#include <qmap.h>
#include "Sql.h"
#include <qvariantmap.h>
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
	
private:
	QMap<int, QTcpSocket*> m_clientsockets;
	void incomingConnection(qintptr socketDesc) override;
	Sql* m_sql;
private slots:
	void handleDatabaseOperation(int operation, QVariantMap parameters, QTcpSocket* socket);
};
