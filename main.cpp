#include "widget.h"

#include <QApplication>
#include <QMessageBox>
#include "namespace.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

if(qAPI::_system::IsRun("DesktopPet")){
    QMessageBox::information(nullptr, "Warning", "The application is already running.");
    return 1;
}
//直接设置自启动

if(!qAPI::_system::IsStartupRegistered("DesktopPet")){
    qAPI::_system::SetCurrentAutoStart("DesktopPet",false,"auto");
}
    Widget w;
    w.show();
    return a.exec();
}
