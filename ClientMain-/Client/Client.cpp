#include "Client.h"
#include <qpixmap.h>
#include <qbuffer.h>
#include <qlistwidget.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlistwidget.h>
#include <qfile.h>
#include <qdir.h>
#include <qlistwidget.h>
#include <qmessagebox.h>
Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientClass())
    , logon_(new LogOn(this))
    , csocket_(new CSocket(this))
{
   
    logon_->show();
 
    csocket_->socket_->connectToHost(QHostAddress(logon_->get_ip()), 8989);
    connect(logon_, &LogOn::send_logon_msg, csocket_, &CSocket::recv_msg);
    connect(csocket_, &CSocket::send_to_logon, logon_, &LogOn::recv_server);
    connect(logon_, &LogOn::send_ok, this, &Client::recv_friend_list);
    connect(csocket_, &CSocket::send_to_client, this, &Client::recv_server);

    connect(this, &Client::friend_list_ok, this, &Client::show_friend_list);
    connect(logon_, &LogOn::send_user, this, [&](User user) {
        user_ = user;
     });
    connect(this, &Client::send_client_msg, csocket_, &CSocket::recv_msg);
    
    ui->setupUi(this);

    connect(ui->listWidget, &QListWidget::itemClicked, this, &Client::friend_listItem_clicked);
    connect(ui->sendBtn, &QPushButton::clicked, this, &Client::clicked_sendBtn);
    connect(this, &Client::send_friend_chatmsg, this, &Client::show_friend_chatMsg);
    connect(ui->searchBtn, &QPushButton::clicked, this, &Client::friend_searchBtn);
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &Client::show_ContextMenu);//好友列表右键菜单
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

Client::~Client()
{
    delete ui;
}

QList<User> Client::json_array_to_list(const QJsonArray& jsonArray)
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

void Client::show_friend_chatMsg(QVariantMap params)
{
    //在当前聊天界面就显示
    if (params.value("friendId").toInt() == user_.id())
    {
        ui->record->append(QString::number(user_.id()) + ":>>>" + params.value("msg").toString());
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
    out << QString::number(user_.id()) + ":>>>" + params.value("msg").toString();
    file.close();
}

void Client::show_ContextMenu(const QPoint& pos)
{
   
    // 获取鼠标点击位置的全局坐标
    QPoint globalPos = mapToGlobal(pos);
    qDebug() << globalPos;
    // 创建一个菜单
    QMenu menu(this);

    // 添加一个“删除”动作
    QAction* deleteAction = menu.addAction("Delete");

    // 连接“删除”动作到槽函数
    connect(deleteAction, &QAction::triggered, this, &Client::remove_SelectedItems);

    // 显示菜单
    menu.exec(globalPos);
}

void Client::remove_SelectedItems()
{
    
    QListWidgetItem* selectedItem = ui->listWidget->currentItem();
    QJsonObject obj;
    obj.insert("head", 5);
    obj.insert("msg", "删除好友");
    obj.insert("userId", user_.id());
    obj.insert("friendId", selectedItem->data(32).toInt());
    qDebug() << selectedItem->data(32).toInt();
    
    if (selectedItem)
    {
        int row = ui->listWidget->row(selectedItem);
        delete ui->listWidget->takeItem(row); // 移除项并释放内
    }
    QJsonDocument doc(obj);
    QString deletefriend = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    emit send_client_msg(deletefriend);

}

//显示用户头像
inline void Client::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
   
    ui->widgetlan->setStyleSheet("background-color: black;");
    ui->icon->setStyleSheet("background-image: url(:/icon.jpg);");
    ui->listWidget->setIconSize(QSize(40, 40));
}

void Client::recv_friend_list(QByteArray friendData)
{
    QJsonParseError jsonError;
    //QBytearray转换为json
    QJsonDocument doc = QJsonDocument::fromJson(friendData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        if (doc.isArray())
        {
            QJsonArray jsonArray = doc.array();
            qDebug() << "JSON Array parsed successfully.123";
            qDebug() << jsonArray;
            friendlist_ = json_array_to_list(jsonArray);
            emit friend_list_ok();
        }
    }
    else
    {
        qDebug() << "Error parsing JSON data:" << jsonError.errorString();

    }
}

void Client::show_friend_list()
{
    for (int i = 0; i < friendlist_.length(); i++)
    {
        QListWidgetItem* newItem = new QListWidgetItem(friendlist_.at(i).name() + ": " + QString::number(friendlist_.at(i).id()));
        newItem->setData(32, friendlist_.at(i).id());
        newItem->setSizeHint(QSize(240, 65));
        newItem->setIcon(QIcon(":/img/2.jpeg"));
        ui->listWidget->addItem(newItem);
        
    }
}

//点击好友显示好友聊天
void Client::friend_listItem_clicked(QListWidgetItem* item)
{
    ui->record->clear();
    qDebug() << "Item clicked:" << item->text() << item->data(32).toInt();
    ui->groupBox->setTitle(item->text());
    friendId_ = item->data(32).toInt();

    QFile file(QString("./UserData/'%1'.txt").arg(friendId_));
    QTextStream in(&file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file for reading:" << file.errorString();
        return;
    }
    while (!in.atEnd()) {
        QString msg = in.readLine();
        ui->record->append("\n" + msg);
    }
    file.close();
}

void Client::clicked_sendBtn()
{
    QString msg = ui->msg->toPlainText();
    QJsonObject obj;
    obj.insert("head", 2);
    obj.insert("msg", msg + "\n");
    obj.insert("userId", user_.id());
    obj.insert("friendId", friendId_);
    QJsonDocument doc(obj);
    QString friendMsg = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
   
    emit send_client_msg(friendMsg);
    ui->record->append(QString::number(user_.id()) + " <<< " + msg + "\n");
    ui->msg->clear();
}

void Client::friend_searchBtn()
{
    int friendId = ui->search->text().toInt();
    QJsonObject obj;
    obj.insert("head", 4);
    obj.insert("userId", user_.id());
    obj.insert("friendId", friendId);
    QJsonDocument doc(obj);
    QString searchfriend = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    emit send_client_msg(searchfriend);
}

//接受服务器发来的信息
void Client::recv_server(int head, QVariantMap params)
{
    switch (head)
    {
    case 2://好友聊天信息
        emit send_friend_chatmsg(params);
        break;

    case 4://添加好友
        if ("friendId", params.value("friendId").toInt() > 0 && "friendName", !params.value("friendName").toString().isEmpty())
        {
            QListWidgetItem* newItem = new QListWidgetItem(params.value("friendName").toString() + ": " + QString::number(params.value("friendId").toInt()));
            newItem->setData(32, params.value("friendId").toInt());
            newItem->setSizeHint(QSize(246, 65));
            newItem->setIcon(QIcon(":/img/1.jpeg"));
            ui->listWidget->addItem(newItem);
            QMessageBox::information(nullptr, "添加成功", "添加成功！");
            break;
        }
        else
        {
            QMessageBox::critical(nullptr, "添加失败", "添加失败，请检查您的输入或稍后再试。");
            break;
        }
    default:
        break;
    }
}
