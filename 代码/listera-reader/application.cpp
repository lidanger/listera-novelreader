#include "application.h"

#include <QIcon>
#include <DApplicationSettings>

Application::Application(int &argc, char **argv)
    :DApplication (argc, argv)
{
    this->setAttribute(Qt::AA_UseHighDpiPixmaps);

    this->setOrganizationName("listera");
    this->setApplicationName("listera-reader");
    this->setApplicationVersion("1.0");
    this->setProductIcon(QIcon(":/images/logo.png"));
    this->setProductName("听雨小说阅读器");
    this->setApplicationDescription("一个 txt 小说阅读器.");
    this->setApplicationHomePage("https://www.listera.top");

    this->setSingleInstance("listera-reader_20200930");

    // 国际化
    this->loadTranslator();

    DApplicationSettings sets(this);
}
