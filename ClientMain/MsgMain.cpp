#include "MsgMain.h"
#include <qpixmap.h>
#include <qbuffer.h>
#include <qlistwidget.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlistwidget.h>
#include <qfile.h>
MsgMain::MsgMain(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MsgMainClass())
{
	ui->setupUi(this);
	ui->widget->setStyleSheet("background-color: black;");
	ui->icon->setStyleSheet("background-image: url(:/icon.jpg);");
	ui->listWidget->setIconSize(QSize(40, 40));

	connect(this, &MsgMain::friendsListOk, this, &MsgMain::showFriendsList);
	connect(ui->listWidget, &QListWidget::itemClicked, this, &MsgMain::onFriendListItemClicked);
	connect(ui->sendBtn, &QPushButton::clicked, this, &MsgMain::onClickedSendBtn);

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


void MsgMain::onFriendListItemClicked(QListWidgetItem* item)
{
	qDebug() << "Item clicked:" << item->text() << item->data(32).toInt();
	ui->groupBox->setTitle(item->text());
	//emit sendFriendIdToMsgBox(item->data(32));
	m_friendid = item->data(32).toInt();
	
	QFile file(QString("./UserData/'%1'.txt").arg(m_friendid));
	QTextStream in(&file);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Failed to open file for reading:" << file.errorString();
		return;
	}
	while (!in.atEnd()) {
		QString msg = in.readLine();
		ui->record->append(msg);
	}
	file.close();
}


void MsgMain::onClickedSendBtn()
{
	QString msg = ui->msg->toPlainText();
	QJsonObject obj;
	obj.insert("head", 2);
	obj.insert("msg", msg + "\n");
	obj.insert("from_id", m_userid);
	obj.insert("to_id", m_friendid);
	QJsonDocument doc(obj);
	QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
	emit sendUserMsgtoFriend(jsonString);
}

void MsgMain::recvUserId(int usrId)
{
	m_userid = usrId;
}

void MsgMain::recvFriendMsg(QVariantMap params)
{
	if (params.value("to_id").toInt() == m_friendid)
	{
		ui->record->append(params.value("msg").toString());
	}
}

void MsgMain::showFriendsList()
{
	for (int i = 0; i < m_friendsList.length(); i++)
	{
		QListWidgetItem* newItem = new QListWidgetItem(m_friendsList.at(i).name());
		newItem->setData(32, m_friendsList.at(i).id());
		newItem->setSizeHint(QSize(246, 65));
		newItem->setIcon(QIcon(":/img/1.jpeg"));
		ui->listWidget->addItem(newItem);
	}
}

void MsgMain::recvFriendsList(QByteArray friendsData)
{
	QJsonParseError jsonError;
	//QBytearray×ª»»Îªjson
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