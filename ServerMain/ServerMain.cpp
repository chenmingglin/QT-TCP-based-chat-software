#include "ServerMain.h"
#include "Server.h"
ServerMain::ServerMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMainClass())
{
    ui->setupUi(this);
    Server* server = new Server(this);
    
}

ServerMain::~ServerMain()
{
    delete ui;
}
