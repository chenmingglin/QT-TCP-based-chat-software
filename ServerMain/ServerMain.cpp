#include "ServerMain.h"
#include "Server.h"
#include "user.h"
#include <qlist.h>
#include <qjsondocument.h>

#include <qjsonarray.h>
ServerMain::ServerMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMainClass())
{
    ui->setupUi(this);
    Server* server = new Server(this);
   /* Sql* sql = new Sql(this);
    QList<User> list = sql->fetchFriendsList(100000);
    for (int i = 0; i < list.length(); i++)
    {
        qDebug() << list.at(i).id() << ":" << list.at(i).name();
    }
    QJsonDocument doc(server->userListToJsonArray(list));
    QByteArray data = doc.toJson();
    qDebug() << QString::fromUtf8(data);*/

}

ServerMain::~ServerMain()
{
    delete ui;
}
