#include "Sql.h"
#include <qdebug.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
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
        QString name = query.value(1).toString();
        qDebug() << "ID:" << id << "Name:" << name;
    }
}

Sql::~Sql()
{}

bool Sql::logon(int user_id, const QString& user_psd)
{
    QSqlQuery query;
    QString sql = QString("SELECT user_psd FROM USER WHERE user_id = '%1';").arg(user_id);
    if (query.exec(sql)) 
    {
        if (query.next()) 
        {
            QString psd = query.value("user_psd").toString();
            qDebug()<< "psd:" << psd;
            return user_psd == query.value("user_psd").toString();
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

bool Sql::login(int* user_id, const QString& user_name, const QString& user_psd, const QString& user_ip)
{
    QSqlQuery query;
    QString sql = QString("insert  into user(user_psd, user_name, user_ip) values('%1', '%2', '%3')")
                         .arg(user_psd).arg(user_name).arg(user_ip);
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
