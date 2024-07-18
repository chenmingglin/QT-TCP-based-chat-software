#include "login.h"

Login::Login(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::loginClass())
{
	ui->setupUi(this);
	connect(ui->ok, &QPushButton::clicked, this, &Login::onClieckedOk);
}

Login::~Login()
{
	delete ui;
}

QString Login::name() const
{
	return ui->name->text();
}

QString Login::psd() const
{
	return ui->psd->text();
}
