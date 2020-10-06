#include "readerconfig.h"
#include <DApplication>
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QTime>
#include <QStandardPaths>

ReaderConfig *ReaderConfig::_inst = nullptr;

ReaderConfig::ReaderConfig()
{
    auto app = static_cast<Dtk::Widget::DApplication *>(QApplication::instance());
    _org_name = app->organizationName();
    _app_name = app->applicationName();
}

void ReaderConfig::addBook(QString book_path)
{
    QFileInfo fi(book_path);
    if (!fi.exists())
    {
        qDebug() << book_path << " not exists" << endl;
        return;
    }

    QString book_name = fi.baseName();

    _setValue("Library", book_name, book_path);
}

void ReaderConfig::setCurrentBook(QString book_name)
{
    _setValue("Reading", "CurrentBook", book_name);

    auto time = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    _setValue("History", time, book_name);
}

void ReaderConfig::addBookmark(QString book_name, int page)
{
    auto key = QString("%1_%2_%3")
                   .arg(book_name)
                   .arg(page)
                   .arg(QDateTime::currentDateTime().toString("MMddHHmmss"));

    _setValue("Bookmark", key, page);
}

void ReaderConfig::setReadingBackgroundPicture(QString file_path)
{
    if (!QFile::exists(file_path))
    {
        qDebug() << file_path << " not exists!" << endl;
        return;
    }

    _setValue("Reading", "BackgroundPicture", file_path);
}
