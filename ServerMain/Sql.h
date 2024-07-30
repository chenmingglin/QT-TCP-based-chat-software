#pragma once

#include <QObject>
#include <Sql.h>
#include <qsqldatabase.h>
#include "user.h"
class Sql  : public QObject
{
	Q_OBJECT

public:
	Sql(QObject *parent);
	~Sql();
	bool logon(int user_id, const QString& user_psd, const QString& user_ip);//����
	bool login(int* user_id, const QString& user_name, const QString& user_psd);
	QList<User> fetchFriendsList(int user_id);
	User addFriend(int user_id, int friend_id);
private:
	QSqlDatabase m_db;
};


