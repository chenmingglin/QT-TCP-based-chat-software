#pragma once

#include <QMainWindow>
#include "ui_MsgMain.h"
#include "User.h"
#include <qlist.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MsgMainClass; };
QT_END_NAMESPACE

class MsgMain : public QMainWindow
{
	Q_OBJECT

public:
	MsgMain(QWidget *parent = nullptr);
	~MsgMain();
	QList<User> jsonArrayToList(const QJsonArray& jsonArray);

private:
	Ui::MsgMainClass *ui;
	QList<User> m_friendsList;
	int m_userid;
	int m_friendid;
signals:
	void friendsListOk();
	void sendUserMsgtoFriend(QByteArray msg);
public slots:
	void recvFriendsList(QByteArray friendsData);
	void showFriendsList();
	void onFriendListItemClicked(QListWidgetItem* item);
	void onClickedSendBtn();
	void recvUserId(int usrId);
	void recvFriendMsg(QVariantMap params);
};
