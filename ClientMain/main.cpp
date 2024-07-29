#include "ClientMain.h"
#include <QtWidgets/QApplication>
#include "MsgMain.h"
int main(int argc, char *argv[])
{
    
    QApplication a(argc, argv);
    ClientMain w;
    MsgMain m;
    w.show();
    return a.exec();
}
