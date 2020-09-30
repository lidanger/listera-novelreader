#ifndef APPLICATION_H
#define APPLICATION_H

#include <DApplication>

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
public:
    Application(int &argc, char **argv);
};

#endif // APPLICATION_H
