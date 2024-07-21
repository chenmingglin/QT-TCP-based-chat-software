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
    setWindowTitle("�ͻ���");
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


//ע��
void ClientMain::onClickedLogin()
{
    Login* login = new Login(this);
    login->setAttribute(Qt::WA_DeleteOnClose); // ��¼�Ի���ر�ʱ�Զ�ɾ��
    login->show();
    //����ע��uiȷ���ź�,��ע��
    connect(login, &Login::onClieckedOk, this, [=] {
        QString name = login->name();
        QString psd = login->psd();

        qDebug() << "name :" << name;
        qDebug() << "psd :" << psd;
        QJsonObject obj;
        obj.insert("head", 3);
        obj.insert("name", name);
        obj.insert("psd", psd);
        obj.insert("msg", "ע��");
        obj.insert("from", 0);
        obj.insert("to", 0);
        obj.insert("ip", getIp());

        QJsonDocument doc(obj);
        QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
        m_socket->write(jsonString);

        connect(this, &ClientMain::sendData, this, &ClientMain::recvData);

        connect(m_socket, &QTcpSocket::errorOccurred, this, [](QAbstractSocket::SocketError err) {
            qDebug() << "�������" << err;
        });
        connect(this, &ClientMain::loginClose, this, [=]() {
            login->close();
            login->deleteLater();
        });
    });
}

//���ܷ���������
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
        //����
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
            int result = QMessageBox::question(nullptr, "Title", "��¼����", QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::Yes)
            {
                // �û�ѡ���ˡ��ǡ�
            }
            else
            {
                // �û�ѡ���ˡ���
            }
        }
        break;
    case 2:
        break;

    case 3:
        //ע��
        if (params.value("flag").toBool())
        {
            int id = params.value("from_id").toInt();
            QMessageBox::information(nullptr, "ע��ɹ�", ("�����˺��ѳɹ�ע�ᣡID:" + QString::number(id)));
            emit loginClose();
        }
        else
        {
            QMessageBox::critical(nullptr, "ע��ʧ��", "ע��ʧ�ܣ���������������Ժ����ԡ�");
            emit loginClose();
        }
        break;

    case 4:
    {
        //�����б�
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



//��¼
void ClientMain::onClickedLogon()
{
    int id = ui->id->text().toInt();
    QString psd = ui->psd->text();
    qDebug() << QString::number(id) + psd;
    
    QJsonObject obj;
    obj.insert("head", 1);
    obj.insert("name", " ");
    obj.insert("psd", psd);
    obj.insert("msg", "����");
    obj.insert("from_id", id);
    obj.insert("to_id", 0);
    obj.insert("ip", getIp());
    
    QJsonDocument doc(obj);
    QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    m_socket->write(jsonString.toUtf8());
    connect(this, &ClientMain::sendData, this, &ClientMain::recvData);
}
