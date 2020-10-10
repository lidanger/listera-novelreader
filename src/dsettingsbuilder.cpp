#include "dsettingsbuilder.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

DSettingsBuilder::DSettingsBuilder()
{

}

DSettingsBuilder::~DSettingsBuilder()
{
    while(!_map.isEmpty())
    {
        auto top_group = _map.last();

        while (!top_group->groups.isEmpty())
        {
            auto group = top_group->groups.last();

            while (!group->options.isEmpty())
            {
                auto option = group->options.last();
                group->options.remove(option->key);
                delete option;
            }

            top_group->groups.remove(group->key);
            delete group;
        }

        _map.remove(top_group->key);
        delete top_group;
    }
}

void DSettingsBuilder::addTopGroup(QString key, QString showName)
{
    auto group = new DSettginsTopGroup;
    group->key = key;
    group->name = showName;

    _map[key] = group;
}

bool DSettingsBuilder::addGroup(QString topGroupKey, QString key, QString showName, bool hide)
{
    if(!_map.contains(topGroupKey))
        return false;

    auto group = new DSettingsGroup;
    group->key = key;
    group->name = showName;
    group->hide = hide;

    _map[topGroupKey]->groups[key] = group;
    return true;
}

bool DSettingsBuilder::addOption(QString topGroupKey, QString groupKey, QString key, QString showName, QVariant defaultValue, QString showType, QVariant max, QVariant min, QString text, bool hide, bool reset)
{
    if(!_map.contains(topGroupKey))
        return false;

    if(!_map[topGroupKey]->groups.contains(groupKey))
        return false;

    auto option = new DSettingsOption;
    option->key = key;
    option->name = showName;
    option->def = defaultValue;
    option->type = showType;
    option->max = max;
    option->min = min;
    option->text = text;
    option->hide = hide;
    option->reset = reset;

    _map[topGroupKey]->groups[groupKey]->options[key] = option;
    return true;
}

QByteArray DSettingsBuilder::toJson(QJsonDocument::JsonFormat format)
{
    QJsonArray topgroups;

    foreach(PDSettginsTopGroup ptg, _map.values())
    {
        QJsonArray groups;

        foreach(PDSettingsGroup pg, ptg->groups)
        {
            QJsonArray options;

            foreach(PDSettingsOption po, pg->options)
            {
                QJsonObject option;
                option.insert("key", po->key);
                option.insert("name", po->name);
                if(!po->type.isEmpty())
                    option.insert("type", po->type);
                if(!po->def.isNull())
                    option.insert("default", QJsonValue::fromVariant(po->def));
                if(!po->max.isNull())
                    option.insert("max", QJsonValue::fromVariant(po->max));
                if(!po->min.isNull())
                    option.insert("min", QJsonValue::fromVariant(po->min));
                if(!po->text.isEmpty())
                    option.insert("text", po->text);
                if(po->hide)
                    option.insert("hide", po->hide);
                if(po->reset)
                    option.insert("reset", po->reset);

                options.append(option);
            }

            QJsonObject group;
            group.insert("key", pg->key);
            group.insert("name", pg->name);
            if(pg->hide)
                group.insert("hide", pg->hide);
            group.insert("options", options);

            groups.append(group);
        }

        QJsonObject topgroup;
        topgroup.insert("key", ptg->key);
        topgroup.insert("name", ptg->name);
        topgroup.insert("groups", groups);

        topgroups.append(topgroup);
    }

    QJsonObject topobj;
    topobj.insert("groups", topgroups);

    QJsonDocument doc(topobj);
    return doc.toJson(format);
}
