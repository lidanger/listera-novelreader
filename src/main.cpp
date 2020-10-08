#include <DApplication>
#include <DApplicationSettings>
#include <DWidgetUtil>
#include "mainwindow.h"
#include "application.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();

    Application a(argc, argv);

    DApplicationSettings savetheme;
    Q_UNUSED(savetheme)

    // 主窗口
    MainWindow mw;
    mw.show();

    Dtk::Widget::moveToCenter(&mw);

    // 消息循环
    return a.exec();
}
