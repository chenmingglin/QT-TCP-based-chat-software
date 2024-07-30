#include "Client.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client w;

    w.logon_->show();
    QObject::connect(&w, &Client::friend_list_ok, &w, [&] {
        w.show();
    });

    return a.exec();
}
