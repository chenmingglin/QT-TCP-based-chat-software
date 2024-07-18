#include "Server.h"
#include <qthread.h>
#include <qdebug.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <Sql.h>
Server::Server(QObject* parent)
	: QTcpServer(parent), m_sql(new Sql(this))
{
	if (this->listen(QHostAddress::Any, 8989))
	{
		QString msg = "ok!";
		qDebug() << msg;
	}
	connect(this, &Server::databaseOperationRequested, this, &Server::handleDatabaseOperation);
}

Server::~Server()
{
}

void Server::disconnected()
{
	QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
	if (!sock)
	{
		return;
	}
	
	m_clientsockets.insert(m_clientsockets.key(sock), nullptr);
	sock->deleteLater();
}

void Server::readReady()
{
	QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
	QString data;
	if (sock && sock->bytesAvailable() > 0) {
		 data = sock->readAll();
		qDebug() << "Received data:" << data;
		
	}
	else
	{
		qDebug() << "read error";
	}
	QJsonDocument doc = doc.fromJson(data.toUtf8());
	QJsonObject obj = doc.object();
	QVariantMap params = obj.toVariantMap();
	int head = params.value("head").toInt();
	switch (head)
	{
		//登出
	case 0:
		emit databaseOperationRequested(0, params, sock);
		break;

		//登入
	case 1:
		emit databaseOperationRequested(1, params, sock);
		break;

	case 2:
		//消息
		emit databaseOperationRequested(2, params, sock);
		break;

	case 3:
		//注册
		emit databaseOperationRequested(3, params,sock);
		break;
	default:
		break;
	}
}

void Server::incomingConnection(qintptr socketDescriptor)
{
	auto* clientSocket = new QTcpSocket(this);
	clientSocket->setSocketDescriptor(socketDescriptor);

	auto* thread = new QThread(this);
	clientSocket->moveToThread(thread);

	connect(clientSocket, &QTcpSocket::readyRead, this, &Server::readReady);
	connect(clientSocket, &QTcpSocket::disconnected, this, &Server::disconnected);
	connect(thread, &QThread::finished, thread, &QThread::deleteLater);
	connect(clientSocket, &QTcpSocket::disconnected, thread, &QThread::quit);
	
	thread->start();
	qDebug() << thread->currentThreadId();
}

void Server::handleDatabaseOperation(int operation, QVariantMap params, QTcpSocket* sock)
{
	int from_id = params.value("from_id").toInt();
	int to_id = params.value("to_id").toInt();
	QString msg = params.value("msg").toString();
	QString name = params.value("name").toString();
	QString psd = params.value("psd").toString();
	QString ip = params.value("ip").toString();
	
	
	switch (operation)
	{
		//登出
	case 0:
		disconnected();
		break;

		//登入
	case 1:
	{
		if (m_sql->logon(from_id, psd))
		{
			QJsonObject obj;
			obj.insert("head", 1);
			obj.insert("msg", "登入成功");
			obj.insert("from_id", from_id);
			obj.insert("flag", true);
			QJsonDocument doc(obj);
			QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
			m_clientsockets.insert(from_id, sock);
			
			// 发送登录成功信息
			sock->write(jsonString.toUtf8());
			sock->flush();
			break;
		}
		else
		{
			qDebug() << "logon error";

			QJsonObject obj;
			obj.insert("head", 1);
			obj.insert("flag", false);

			QJsonDocument doc(obj);
			QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));

			// 发送登录失败信息
			sock->write(jsonString.toUtf8());
			sock->flush();
			break;
		}
	}

	case 2:
	{ //消息
	  auto it = m_clientsockets.find(to_id);
	  if (it != m_clientsockets.end())
	  {
		  it.value()->write(name.toUtf8() +"："+ msg.toUtf8());
		  break;
	  }
	  break;
	}
	//注册
	case 3:
		qDebug() << "name :" <<name;
		qDebug() << "psd :" << psd;
		if (m_sql->login(&from_id, name, psd, ip))
		{
			QJsonObject obj;
			obj.insert("head", 3);
			obj.insert("msg", "注册成功");
			obj.insert("from_id", from_id);
			obj.insert("flag", true);
			QJsonDocument doc(obj);
			QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
			sock->write(jsonString.toUtf8());
			sock->flush();
			break;
		}
		else
		{
			qDebug() << "login error";
			QJsonObject obj;

			obj.insert("head", 3);
			obj.insert("flag", false);
			
			QJsonDocument doc(obj);
			QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));

			//发送注册失败信息
			sock->write(jsonString.toUtf8());
			sock->flush();
			break;
		}
		break;
	default:
		break;
	}
}

