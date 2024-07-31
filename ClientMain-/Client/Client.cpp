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
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &Client::show_ContextMenu);//�����б��Ҽ��˵�
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
    //�ڵ�ǰ����������ʾ
    if (params.value("friendId").toInt() == user_.id())
    {
        ui->record->append(QString::number(user_.id()) + ":>>>" + params.value("msg").toString());
    }
    // �����ļ�·��
    QDir dir(".");
    dir.cd("UserData"); // ���Խ���UserDataĿ¼

    // ���UserDataĿ¼�����ڣ��򴴽�֮
    if (!dir.exists()) {
        if (!dir.mkdir("UserData")) { // ����Ŀ¼
            qWarning() << "error!!! Unable to create UserData directory.";

        }
        dir.cd("UserData"); // �ٴγ��Խ���UserDataĿ¼
    }
    // ָ���ļ���
    QString fileName = dir.absoluteFilePath(QString("'%1'.txt").arg(params.value("userId").toInt()));
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open file for appending:" << file.errorString();
        return;
    }
    // ʹ��QTextStream׷������
    QTextStream out(&file);
    out << QString::number(user_.id()) + ":>>>" + params.value("msg").toString();
    file.close();
}

void Client::show_ContextMenu(const QPoint& pos)
{
   
    // ��ȡ�����λ�õ�ȫ������
    QPoint globalPos = mapToGlobal(pos);
    qDebug() << globalPos;
    // ����һ���˵�
    QMenu menu(this);

    // ���һ����ɾ��������
    QAction* deleteAction = menu.addAction("Delete");

    // ���ӡ�ɾ�����������ۺ���
    connect(deleteAction, &QAction::triggered, this, &Client::remove_SelectedItems);

    // ��ʾ�˵�
    menu.exec(globalPos);
}

void Client::remove_SelectedItems()
{
    
    QListWidgetItem* selectedItem = ui->listWidget->currentItem();
    QJsonObject obj;
    obj.insert("head", 5);
    obj.insert("msg", "ɾ������");
    obj.insert("userId", user_.id());
    obj.insert("friendId", selectedItem->data(32).toInt());
    qDebug() << selectedItem->data(32).toInt();
    
    if (selectedItem)
    {
        int row = ui->listWidget->row(selectedItem);
        delete ui->listWidget->takeItem(row); // �Ƴ���ͷ���
    }
    QJsonDocument doc(obj);
    QString deletefriend = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    emit send_client_msg(deletefriend);

}

//��ʾ�û�ͷ��
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
    //QBytearrayת��Ϊjson
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

//���������ʾ��������
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

//���ܷ�������������Ϣ
void Client::recv_server(int head, QVariantMap params)
{
    switch (head)
    {
    case 2://����������Ϣ
        emit send_friend_chatmsg(params);
        break;

    case 4://��Ӻ���
        if ("friendId", params.value("friendId").toInt() > 0 && "friendName", !params.value("friendName").toString().isEmpty())
        {
            QListWidgetItem* newItem = new QListWidgetItem(params.value("friendName").toString() + ": " + QString::number(params.value("friendId").toInt()));
            newItem->setData(32, params.value("friendId").toInt());
            newItem->setSizeHint(QSize(246, 65));
            newItem->setIcon(QIcon(":/img/1.jpeg"));
            ui->listWidget->addItem(newItem);
            QMessageBox::information(nullptr, "��ӳɹ�", "��ӳɹ���");
            break;
        }
        else
        {
            QMessageBox::critical(nullptr, "���ʧ��", "���ʧ�ܣ���������������Ժ����ԡ�");
            break;
        }
    default:
        break;
    }
}
