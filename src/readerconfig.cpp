#include "dsettingsbuilder.h"
#include "readerconfig.h"
#include <DApplication>
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QTime>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

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
        qDebug() << book_path << QObject::tr(" not exists") << endl;
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

void ReaderConfig::addBookmark(QString book_name, int page, QString page_title)
{
    auto key = QString("%1_%2_%3")
            .arg(book_name)
            .arg(page_title)
            .arg(page);

    _setValue("Bookmark", key, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
}

void ReaderConfig::setTextColor(QColor color)
{
    _setValue("Reading", "TextColorR", color.red());
    _setValue("Reading", "TextColorG", color.green());
    _setValue("Reading", "TextColorB", color.blue());
    _setValue("Reading", "TextColorA", color.alpha());
}

QColor ReaderConfig::textColor()
{
    QColor color;
    color.setRed(_getValue("Reading", "TextColorR").toInt());
    color.setGreen(_getValue("Reading", "TextColorG").toInt());
    color.setBlue(_getValue("Reading", "TextColorB").toInt());
    color.setAlpha(_getValue("Reading", "TextColorA").toInt());
    return color;
}

void ReaderConfig::setBackgroundColor(QColor color)
{
    _setValue("Reading", "BackgroundColorR", color.red());
    _setValue("Reading", "BackgroundColorG", color.green());
    _setValue("Reading", "BackgroundColorB", color.blue());
    _setValue("Reading", "BackgroundColorA", color.alpha());
}

QColor ReaderConfig::backgroundColor()
{
    QColor color;
    color.setRed(_getValue("Reading", "BackgroundColorR").toInt());
    color.setGreen(_getValue("Reading", "BackgroundColorG").toInt());
    color.setBlue(_getValue("Reading", "BackgroundColorB").toInt());
    color.setAlpha(_getValue("Reading", "BackgroundColorA").toInt());
    return color;
}

void ReaderConfig::setBackgroundPicture(QString file_path)
{
    if (!QFile::exists(file_path))
    {
        qDebug() << file_path << QObject::tr(" not exists") << endl;
        return;
    }

    _setValue("Reading", "BackgroundPicture", file_path);
}

QByteArray ReaderConfig::getDSettingsJson()
{
    QSettings sets(_org_name, _app_name);
    DSettingsBuilder sb;

    QString topGroup = "basic";
    sb.addTopGroup(topGroup, topGroup);

    QSet<QString> blacklist({"History", "Bookmark", "Library"});
    auto groups = sets.childGroups();
    foreach(QString group, groups)
    {
        if(blacklist.contains(group))
            continue;

        sb.addGroup(topGroup, group, group);

        sets.beginGroup(group);

        auto keys = sets.childKeys();
        foreach(QString key, keys)
        {
            auto val = sets.value(key);
            if(group == "Shotcuts")
                sb.addOption(topGroup, group, key, key, val, "shotcut");
            else
                sb.addOption(topGroup, group, key, key, val, "lineedit");
        }

        sets.endGroup();
    }

    return sb.toJson();
}
