#include "MsgMain.h"
#include <qpixmap.h>
#include <qbuffer.h>
#include <qlistwidget.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlistwidget.h>
#include <qfile.h>
#include <qdir.h>

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
	ui->record->clear();
	qDebug() << "Item clicked:" << item->text() << item->data(32).toInt();
	ui->groupBox->setTitle(item->text());
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
	obj.insert("userId", m_userid);
	obj.insert("friendId", m_friendid);
	QJsonDocument doc(obj);
	QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
	emit sendUserMsgtoFriend(jsonString);
	ui->record->setAlignment(Qt::AlignRight);
	ui->record->append(QString::number(m_userid) + ":" + "\n" + msg);
	ui->msg->clear();
}

void MsgMain::recvUserId(int usrId)
{
	m_userid = usrId;
}

void MsgMain::recvFriendMsg(QVariantMap params)
{
	if (params.value("friendId").toInt() == m_userid)
	{
		ui->record->append(QString::number(m_userid) + ":\n\t" + params.value("msg").toString());
	}
	// 构建文件路径
	QDir dir(".");
	dir.cd("UserData"); // 尝试进入UserData目录

	// 如果UserData目录不存在，则创建之
	if (!dir.exists()) {
		if (!dir.mkdir("UserData")) { // 创建目录
			qWarning() << "error!!! Unable to create UserData directory.";

		}
		dir.cd("UserData"); // 再次尝试进入UserData目录
	}
	// 指定文件名
	QString fileName = dir.absoluteFilePath(QString("'%1'.txt").arg(params.value("userId").toInt()));
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
		qWarning() << "Failed to open file for appending:" << file.errorString();
		return;
	}
	// 使用QTextStream追加数据
	QTextStream out(&file);
	out << QString::number(m_userid) + "\n \t" + params.value("msg").toString();
	file.close();
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