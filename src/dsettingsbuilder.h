#ifndef DSETTINGSUTILS_H
#define DSETTINGSUTILS_H

#include <QJsonDocument>
#include <QMap>
#include <QString>
#include <QVariant>

class DSettingsBuilder
{
public:
    DSettingsBuilder();
    ~DSettingsBuilder();

public:
    void addTopGroup(QString key, QString showName);
    bool addGroup(QString topGroupKey, QString key, QString showName, bool hide = false);
    bool addOption(QString topGroupKey, QString groupKey, QString key, QString showName, QVariant defaultValue, QString showType = QString(), QVariant max = QVariant(), QVariant min = QVariant(), QString text = QString(), bool hide = false, bool reset = false);

    QByteArray toJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact);

private:
    // 选项
    typedef struct DSettingsOption
    {
        QString key;
        // 显示名称
        QString name;
        // 显示控件类型
        QString type;
        // 默认值
        QVariant def;
        QVariant max;
        QVariant min;
        QString text;
        // 隐藏此项
        bool hide;
        // 可重置
        bool reset;
    } *PDSettingsOption;

    // 选项组
    typedef struct DSettingsGroup
    {
        QString key;
        QString name;
        QMap<QString, PDSettingsOption> options;
        // 隐藏此组
        bool hide;
    } *PDSettingsGroup;

    // 顶层选项组
    typedef struct DSettginsTopGroup
    {
        QString key;
        QString name;
        QMap<QString, PDSettingsGroup> groups;
    } *PDSettginsTopGroup;

private:
    QMap<QString, PDSettginsTopGroup> _map;
};

#endif // DSETTINGSUTILS_H
