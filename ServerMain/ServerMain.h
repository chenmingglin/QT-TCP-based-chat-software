#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ServerMain.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ServerMainClass; };
QT_END_NAMESPACE

class ServerMain : public QMainWindow
{
    Q_OBJECT

public:
    ServerMain(QWidget *parent = nullptr);
    ~ServerMain();

private:
    Ui::ServerMainClass *ui;
};
