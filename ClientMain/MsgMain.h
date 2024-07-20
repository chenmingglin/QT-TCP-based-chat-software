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

signals:
	void friendsListOk();
public slots:
	void recvFriendsList(QByteArray friendsData);
	void showFriendsList();
};
