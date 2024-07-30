#include "LogOn.h"
#include <qhostaddress.h>
#include <QNetworkInterface>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qvariantmap.h>
#include <qmessagebox.h>
#include <qwidget.h>
LogOn::LogOn(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::LogOnClass()), login_(new Login(this)), isLogon_(false)
{
	ui->setupUi(this);
    connect(login_, &Login::send_login_msg, this, &LogOn::send_logon_msg);
}

LogOn::~LogOn()
{
	delete ui;
}

QString LogOn::get_ip() const
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

void LogOn::on_login_clicked()
{
    login_->show();
}

void LogOn::recv_login_msg(QString loginMsg)
{
    emit send_logon_msg(loginMsg);
}

void LogOn::recv_server(int head, QVariantMap params)
{
    switch (head)
    {
    case 1:
    {
        isLogon_ = false;
        if (params.value("flag").toBool() == true && isLogon_ == false)
        {
            isLogon_ = true;
            //emit sendUserId(params.value("userId").toInt());
            QByteArray friendsData = params.value("msg").toByteArray();
            qDebug() << "logon ok:";
            this->hide();
            emit send_ok(friendsData);
            break;
        }
        else
        {
            isLogon_ = false;
            qDebug() << params.value("msg").toByteArray();
            qDebug() << "logon error";
            int result = QMessageBox::question(nullptr, "Title", "µÇÂ¼´íÎó", QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::Yes)
            {
                
                // ÓÃ»§Ñ¡ÔñÁË¡°ÊÇ¡±
                break;
            }
            else
            {
                
                
                // ÓÃ»§Ñ¡ÔñÁË¡°·ñ¡±
                break;
            }
        }
    }

    case 3://×¢²á
        if (params.value("flag").toBool())
        {
            int id = params.value("userId").toInt();
            //QMessageBox::information(nullptr, "×¢²á³É¹¦", ("ÄúµÄÕËºÅÒÑ³É¹¦×¢²á£¡ID:" + QString::number(id)));
            login_->ui->id->setText(QString::number(id));
            
            break;
        }
        else
        {
            //QMessageBox::critical(nullptr, "×¢²áÊ§°Ü", "×¢²áÊ§°Ü£¬Çë¼ì²éÄúµÄÊäÈë»òÉÔºóÔÙÊÔ¡£");
            login_->ui->id->setText("×¢²áÊ§°Ü!");
            break;
        }
    default:
        break;
    }
}

void LogOn::on_logon_clicked()
{
    int id = ui->id->text().toInt();
    QString psd = ui->psd->text();
    qDebug() << QString::number(id) + psd;

    QJsonObject obj;
    obj.insert("head", 1);
    obj.insert("name", " ");
    obj.insert("psd", psd);
    obj.insert("msg", "µÇÈë");
    obj.insert("userId", id);

    obj.insert("ip", get_ip());

    QJsonDocument doc(obj);
    QString logonMsg = QString::fromUtf8(doc.toJson());
    emit send_logon_msg(logonMsg);
}