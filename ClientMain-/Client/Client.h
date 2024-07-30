#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ClientClass; };
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

private:
    Ui::ClientClass *ui;
};
