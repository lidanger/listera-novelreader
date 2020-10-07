#include "application.h"

#include <QIcon>
#include <QTranslator>
#include <QDir>
#include <QDebug>
#include <QTime>
#include <QStandardPaths>
#include <QTextCodec>

Application::Application(int &argc, char **argv)
    :DApplication (argc, argv)
{
    this->setAttribute(Qt::AA_UseHighDpiPixmaps);

    // 国际化
    this->loadTranslator();

    this->setOrganizationName("listera");
    this->setApplicationName("listera-reader");
    this->setApplicationVersion("1.0.1.2");
    this->setProductIcon(QIcon(":/images/logo.png"));
    this->setProductName(tr("Listera Reader"));
    this->setApplicationDescription(tr("A txt novel reader."));
    this->setApplicationHomePage("https://www.listera.top");

    this->setSingleInstance("listera-reader_20200930");

#ifdef QT_DEBUG
    QTranslator trans;
    if(trans.load(QLocale(), "listera-reader", "_", QDir::currentPath(), ".qm"))
    {
        qDebug() << "load qm" << QLocale() << endl;
        this->installTranslator(&trans);
    }
#endif
}

void Application::setMessageLogging()
{
#ifdef QT_NO_DEBUG
    qInstallMessageHandler(_messageOutput);
#endif
}

void Application::_messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString text;

    QByteArray localMsg = msg.toLocal8Bit();
    switch (type)
    {
    case QtDebugMsg:
        text.sprintf("Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        text.sprintf("Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        text.sprintf("Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        text.sprintf("Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        text.sprintf("Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    }

    // 设置输出信息格式
    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString message = QString("%1 %2 (%3)").arg(text).arg(context_info).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"));

    // 输出信息至文件中（读写、追加形式）
    auto cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QFileInfo fi(cacheDir + "/program.log");
    if(!fi.dir().exists())
        fi.dir().mkpath(fi.absolutePath());

    QFile file(fi.absoluteFilePath());
    if(file.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << message << "\r\n";

        file.flush();
        file.close();
    }
}
