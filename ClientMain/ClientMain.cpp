#include "ClientMain.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>

#include <qjsonvalue.h>
#include <qmessagebox.h>
#include <qvariantmap.h>
#include <qlist.h>
#include "user.h"

ClientMain::ClientMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainClass())
{
    ui->setupUi(this);
    m_socket = new QTcpSocket(this);
    m_msgWidget = new MsgMain(this);
    setWindowTitle("客户端");
    m_socket->connectToHost(QHostAddress(getIp()), 8989);
    connect(ui->logon, &QPushButton::clicked, this, &ClientMain::onClickedLogon);
    connect(ui->login, &QPushButton::clicked, this, &ClientMain::onClickedLogin);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientMain::recvSocketData);
    connect(this, &ClientMain::sendFriendsList, m_msgWidget, &MsgMain::recvFriendsList);
    connect(this, &ClientMain::sendLogonOk, this, &ClientMain::recvLogonOk);
    connect(this, &ClientMain::sendUserId, m_msgWidget, &MsgMain::recvUserId);
    connect(m_msgWidget, &MsgMain::sendUserMsgtoFriend, this, &ClientMain::recvUserMsgtoFriend);
}

ClientMain::~ClientMain()
{
    delete ui;
}

QString ClientMain::getIp() const
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    foreach(const QNetworkInterface & interface, interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            foreach(const QNetworkAddressEntry & entry, entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    qDebug() << "Interface:" << interface.humanReadableName();
                    qDebug() << "IP Address:" << entry.ip().toString();
                    return entry.ip().toString();
                }
            }
        }
    }
    return QString();
}


//注册
void ClientMain::onClickedLogin()
{
    Login* login = new Login(this);
    login->setAttribute(Qt::WA_DeleteOnClose); // 登录对话框关闭时自动删除
    login->show();
    //接受注册ui确认信号,并注册
    connect(login, &Login::onClieckedOk, this, [=] {
        QString name = login->name();
        QString psd = login->psd();

        qDebug() << "name :" << name;
        qDebug() << "psd :" << psd;
        QJsonObject obj;
        obj.insert("head", 3);
        obj.insert("name", name);
        obj.insert("psd", psd);
        obj.insert("msg", "注册");
        obj.insert("from", 0);
        obj.insert("to", 0);
        obj.insert("ip", getIp());

        QJsonDocument doc(obj);
        QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
        m_socket->write(jsonString);

        connect(this, &ClientMain::sendData, this, &ClientMain::recvData);

        connect(m_socket, &QTcpSocket::errorOccurred, this, [](QAbstractSocket::SocketError err) {
            qDebug() << "网络错误：" << err;
        });
        connect(this, &ClientMain::loginClose, this, [=]() {
            login->close();
            login->deleteLater();
        });
    });
}

//接受服务器数据
void ClientMain::recvSocketData()
{
    QByteArray data = m_socket->readAll();
    QJsonDocument doc = doc.fromJson(data);
    QJsonObject obj = doc.object();

    QVariantMap params = obj.toVariantMap();
    int head = params.value("head").toInt();
    emit sendData(head, params);
}

void ClientMain::recvData(int operation,QVariantMap params)
{
    switch (operation)
    {
    case 0:
        break;
    case 1:
        //登入
        if (params.value("flag").toBool())
        {
            emit sendLogonOk();
            emit sendUserId(params.value("from_id").toInt());
            qDebug() << "logon ok:" << params.value("msg").toString();
            this->close();
            m_msgWidget->show();
            
        }
        else
        {
            qDebug() << "logon error" << params.value("msg").toString();;
            int result = QMessageBox::question(nullptr, "Title", "登录错误", QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::Yes)
            {
                // 用户选择了“是”
            }
            else
            {
                // 用户选择了“否”
            }
        }
        break;
    case 2:
        break;

    case 3:
        //注册
        if (params.value("flag").toBool())
        {
            int id = params.value("from_id").toInt();
            QMessageBox::information(nullptr, "注册成功", ("您的账号已成功注册！ID:" + QString::number(id)));
            emit loginClose();
        }
        else
        {
            QMessageBox::critical(nullptr, "注册失败", "注册失败，请检查您的输入或稍后再试。");
            emit loginClose();
        }
        break;

    case 4:
    {
        //好友列表
        QByteArray friendsData = params.value("msg").toByteArray();
        emit sendFriendsList(friendsData);
        break;
    }
    default:
        break;
    }
}

void ClientMain::recvLogonOk()
{
    int id = ui->id->text().toInt();
    QJsonObject obj;
    obj.insert("head", 4);
    obj.insert("from_id", id);

    QJsonDocument doc(obj);
    QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
    m_socket->write(jsonString);
}

void ClientMain::recvUserMsgtoFriend(QByteArray msg)
{
    m_socket->write(msg);
}



//登录
void ClientMain::onClickedLogon()
{
    int id = ui->id->text().toInt();
    QString psd = ui->psd->text();
    qDebug() << QString::number(id) + psd;
    
    QJsonObject obj;
    obj.insert("head", 1);
    obj.insert("name", " ");
    obj.insert("psd", psd);
    obj.insert("msg", "登入");
    obj.insert("from_id", id);
    obj.insert("to_id", 0);
    obj.insert("ip", getIp());
    
    QJsonDocument doc(obj);
    QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    m_socket->write(jsonString.toUtf8());
    connect(this, &ClientMain::sendData, this, &ClientMain::recvData);
}
