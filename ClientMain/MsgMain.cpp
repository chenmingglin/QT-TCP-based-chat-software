#include "MsgMain.h"
#include <qpixmap.h>
#include <qbuffer.h>
#include <qlistwidget.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
MsgMain::MsgMain(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MsgMainClass())
{
	ui->setupUi(this);
	ui->widget->setStyleSheet("background-color: black;");
	ui->icon->setStyleSheet("background-image: url(:/icon.jpg);");
	connect(this, &MsgMain::friendsListOk, this, &MsgMain::showFriendsList);
	/*QListWidgetItem* newItem = new QListWidgetItem("张三");
	newItem->setSizeHint(QSize(246, 65));
	ui->listWidget->addItem(newItem);*/

}

MsgMain::~MsgMain()
{
	delete ui;
}

QList<User> MsgMain::jsonArrayToList(const QJsonArray& jsonArray)
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


void MsgMain::showFriendsList()
{
	for (int i = 0; i < m_friendsList.length(); i++)
	{
		QListWidgetItem* newItem = new QListWidgetItem(m_friendsList.at(i).name());
		newItem->setSizeHint(QSize(246, 65));
		ui->listWidget->addItem(newItem);
	}
}

void MsgMain::recvFriendsList(QByteArray friendsData)
{
	QJsonParseError jsonError;
	//QBytearray转换为json
	QJsonDocument doc = QJsonDocument::fromJson(friendsData, &jsonError);
	if (jsonError.error == QJsonParseError::NoError)
	{
		if (doc.isArray())
		{
			QJsonArray jsonArray = doc.array();
			qDebug() << "JSON Array parsed successfully.123";
			qDebug() << jsonArray;
			m_friendsList = jsonArrayToList(jsonArray);
			emit friendsListOk();
		}
	}
	else 
	{
		qDebug() << "Error parsing JSON data:" << jsonError.errorString();
		
	}

}