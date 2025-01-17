#include "Sql.h"
#include <qdebug.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qlist.h>
Sql::Sql(QObject *parent)
	: QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");

    // 设置数据库连接参数
    m_db.setHostName("localhost");      // 数据库服务器的地址
    m_db.setDatabaseName("users"); // 数据库名称
    m_db.setUserName("root");           // 数据库用户名
    m_db.setPassword("Chen021204.");       // 数据库密码
    if (!m_db.open()) {
        qDebug() << "Cannot open database" << m_db.lastError().text();
        return;
    }
    QSqlQuery query;
    // 准备并执行SQL语句
    if (!query.exec("SELECT * FROM user")) {
        qDebug() << "Error executing query:" << query.lastError().text();
        return;
    }
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value("user_name").toString();
        qDebug() << "ID:" << id << "Name:" << name;
    }
}

Sql::~Sql()
{}

bool Sql::logon(User* user)
{
    QSqlQuery query;
    QString sql = QString("SELECT user_psd, user_ip FROM USER WHERE user_id = '%1';").arg(user->id());
    if (query.exec(sql)) 
    {
        if (query.next()) 
        {
            QString psd = query.value("user_psd").toString();
            qDebug()<< "psd:" << psd;
            QString ip = query.value("user_ip").toString();
            qDebug() << "ip:" << ip;
            QString name = query.value("user_name").toString();
            user->set_name(name);

            if (user->ip() != query.value("user_ip").toString())
            {
                QString sql2 = QString("UPDATE USER SET user_ip = '%1' WHERE user_id = '%2'").arg(user->ip()).arg(user->id());
                bool flag = m_db.transaction();
                if (flag)
                {
                    if (query.exec(sql2))
                    {
                        m_db.commit();
                        
                    }
                    else
                    {
                        m_db.rollback();
                        qDebug() << "Commt error executing logon ip query2:" << query.lastError().text();
                    }
                }

            }
            return user->psd() == psd;
        }
        else
        {
            qDebug() << "Error executing logon query.next():" << query.lastError().text();
            return false;
        }
    }
    else
    {
        qDebug() << "Error executing logon query:" << query.lastError().text();
        return false;
    }
   
}

bool Sql::login(int* user_id, const QString& user_name, const QString& user_psd)
{
    QSqlQuery query;
    QString sql = QString("insert  into user(user_psd, user_name, user_ip) values('%1', '%2', '%3')")
        .arg(user_psd).arg(user_name).arg(" ");
    bool flag = m_db.transaction();
    if (flag)
    {
        if (query.exec(sql))
        {
            *user_id = query.lastInsertId().toInt();
            m_db.commit();
            return true;
        }
        else
        {
            m_db.rollback();
            qDebug() << "Commt error executing login  query:" << query.lastError().text();
            return false;
        }
    }
    else
    {
        qDebug() << "error executing login  query:" << query.lastError().text();
        return false;
    }
    
}

QList<User> Sql::fetchFriendsList(int user_id)
{
    QSqlQuery query;
    
    QList<User> friendsList;
    QString sql = QString(
        "SELECT "
        "fu.user_id AS friend_id,"
        "fu.user_name AS friend_username "
        "FROM "
        "USER u "
        "JOIN "
        "friends f ON u.user_id = f.user_id "
        "JOIN "
        "USER fu ON fu.user_id = f.friend_id "
        "WHERE "
        "u.user_id = '%1' AND f.status = 'accepted';"
    ).arg(user_id);

    if (!query.exec(sql)) {
        qDebug() << "Error executing friend query:" << query.lastError().text();
        return QList<User>();
    }
    while (query.next()) {
        int id = query.value("friend_id").toInt();
        QString name = query.value("friend_username").toString();
        qDebug() << name;
        User friendInfo(id, name);
        friendsList.append(friendInfo);
    }
    return friendsList;
}

User Sql::addFriend(int user_id, int friend_id)
{
    User friendInfo(friend_id, "");
    QSqlQuery query;
    QString sql1 = QString("INSERT INTO friends(user_id, friend_id, status) VALUES('%1', '%2', 'accepted');").arg(user_id).arg(friend_id);
    QString sql2 = QString("INSERT INTO friends(user_id, friend_id, status) VALUES('%1', '%2', 'accepted');").arg(friend_id).arg(user_id);
    bool flag2 = m_db.transaction();
    if (flag2)
    {
        if (query.exec(sql2))
        {
            m_db.commit();

        }
        else
        {
            m_db.rollback();
            qDebug() << "Commt error executing addFriend2  query:" << query.lastError().text();
        }
    }
    bool flag1 = m_db.transaction();
    if (flag1)
    {

        if (query.exec(sql1))
        {
            m_db.commit();
            QString sql2 = QString("SELECT user_name FROM USER WHERE user_id = '%1';").arg(friend_id);
            if (query.exec(sql2))
            {
                if (query.next())
                {
                    QString name = query.value("user_name").toString();
                    friendInfo.set_name(name);
                    return friendInfo;
                }
                else
                {
                    qDebug() << "Error executing addFriend query.next():" << query.lastError().text();
                    return friendInfo;
                }
            }
        }
        else
        {
            m_db.rollback();
            qDebug() << "Commt error executing addFriend  query:" << query.lastError().text();
            return friendInfo;
        }
    }
   
    
}

bool Sql::delFriend(int user_id, int friend_id)
{
    QSqlQuery query;
    QString sql = QString("UPDATE friends SET STATUS = 'pending' WHERE user_id = '%1' AND friend_id = '%2'").arg(user_id).arg(friend_id);

    bool flag = m_db.transaction();
    if (flag)
    {
        if (query.exec(sql))
        {
            m_db.commit();
            qDebug() << "del success!";
            return true;
        }
        else
        {
            m_db.rollback();
            qDebug() << "del error!";
            return false;
        }
    }
    else
    {
        return false;
    }
}



