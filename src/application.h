#ifndef APPLICATION_H
#define APPLICATION_H

#include <DApplication>

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);

public:
    static void setMessageLogging();

private slots:
    void _aboutToQuit();

private:
    static void _messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

};

#endif // APPLICATION_H
