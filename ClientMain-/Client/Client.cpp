#include "Client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientClass())
{
    ui->setupUi(this);
}

Client::~Client()
{
    delete ui;
}
