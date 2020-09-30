#include <DApplication>
#include <DWidgetUtil>
#include "mainwindow.h"
#include "application.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();

    Application a(argc, argv);

    // 主窗口
    MainWindow mw;
    mw.show();

    Dtk::Widget::moveToCenter(&mw);

    return a.exec();
}
