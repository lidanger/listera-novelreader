#include "readerconfig.h"
#include "shotcutsettings.h"

#include <QBoxLayout>
#include <QListWidget>
#include <DLabel>
#include <QResizeEvent>
#include <DMainWindow>
#include <DStatusBar>
#include <DTitlebar>
#include <QToolButton>
#include <QToolBar>
#include <QKeySequenceEdit>

ShotcutSettings::ShotcutSettings(QWidget *parent)
    :DDialog (parent)
{
    init_ui();
    init_data();
}

void ShotcutSettings::init_ui()
{
    this->setWindowTitle(tr("Shotcuts"));
    this->setFixedSize(370, 600);

    this->addButton(tr("OK"));
    this->addButton(tr("Cancel"), true);
    connect(this, SIGNAL(buttonClicked(int, const QString &)), this, SLOT(onButtonClicked(int, const QString &)));

    _shortcut_list = new QListWidget(this);
    this->addContent(_shortcut_list);

    _shortcut_list->setSelectionMode(QAbstractItemView::NoSelection);
}

void ShotcutSettings::init_data()
{
    if(this->parent() == nullptr)
        return;

    // 搜索主窗口的可配置项
    auto mainwindow = static_cast<DMainWindow*>(this->parent());

    // 标题栏按钮
    auto topbar_title = new QListWidgetItem(tr("Titlebar buttons"), _shortcut_list);
    _shortcut_list->addItem(topbar_title);
    topbar_title->setSizeHint(QSize(100, 50));
    auto font = topbar_title->font();
    font.setPointSize(12);
    topbar_title->setFont(font);

    auto topbar_buttons = mainwindow->titlebar()->findChildren<QToolButton*>();
    foreach(QToolButton *button, topbar_buttons)
    {
        auto item = new QListWidgetItem(_shortcut_list);
        _shortcut_list->addItem(item);
        item->setSizeHint(QSize(100, 50));
        item->setData(Qt::UserRole, button->objectName());

        auto widget = new QWidget(_shortcut_list);
        _shortcut_list->setItemWidget(item, widget);

        auto layout = new QBoxLayout(QBoxLayout::LeftToRight, widget);
        widget->setLayout(layout);

        auto label = new DLabel(button->text(), widget);
        label->setFixedWidth(100);
        layout->addWidget(label);

        auto edit = new QKeySequenceEdit(widget);
        layout->addWidget(edit);

        edit->setKeySequence(button->shortcut());
    }

    // 工具栏
    auto toolbars_title = new QListWidgetItem(tr("Toolbar buttons"), _shortcut_list);
    _shortcut_list->addItem(toolbars_title);
    toolbars_title->setSizeHint(QSize(100, 50));
    toolbars_title->setFont(font);

    auto toolbars = mainwindow->findChildren<DToolBar*>();
    foreach(QToolBar *toolbar, toolbars)
    {
        auto toolbar_buttons = toolbar->findChildren<QToolButton*>();
        foreach(QToolButton *button, toolbar_buttons)
        {
            if(button->text().isEmpty())
                continue;

            auto item = new QListWidgetItem(_shortcut_list);
            _shortcut_list->addItem(item);
            item->setSizeHint(QSize(100, 50));
            item->setData(Qt::UserRole, button->defaultAction()->objectName());

            auto widget = new QWidget(_shortcut_list);
            _shortcut_list->setItemWidget(item, widget);

            auto layout = new QBoxLayout(QBoxLayout::LeftToRight, widget);
            widget->setLayout(layout);

            auto label = new DLabel(button->text(), widget);
            label->setFixedWidth(100);
            layout->addWidget(label);

            auto edit = new QKeySequenceEdit(widget);
            layout->addWidget(edit);

            if(button->defaultAction()->objectName() == "bosskey")
                edit->setKeySequence(mainwindow->findChild<QAction*>("bosskey_real")->shortcut());
            else
                edit->setKeySequence(button->defaultAction()->shortcut());
        }
    }

    // 状态栏
    auto statusbar_title = new QListWidgetItem(tr("Statusbar buttons"), _shortcut_list);
    _shortcut_list->addItem(statusbar_title);
    statusbar_title->setSizeHint(QSize(100, 50));
    statusbar_title->setFont(font);

    auto statusbar_buttons = mainwindow->statusBar()->findChildren<QToolButton*>();
    foreach(QToolButton *button, statusbar_buttons)
    {
        auto item = new QListWidgetItem(_shortcut_list);
        _shortcut_list->addItem(item);
        item->setSizeHint(QSize(100, 50));
        item->setData(Qt::UserRole, button->defaultAction()->objectName());

        auto widget = new QWidget(_shortcut_list);
        _shortcut_list->setItemWidget(item, widget);

        auto layout = new QBoxLayout(QBoxLayout::LeftToRight, widget);
        widget->setLayout(layout);

        auto label = new DLabel(button->text(), widget);
        label->setFixedWidth(100);
        layout->addWidget(label);

        auto edit = new QKeySequenceEdit(widget);
        layout->addWidget(edit);

        edit->setKeySequence(button->defaultAction()->shortcut());
    }
}

void ShotcutSettings::onButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text)

    if(index != 0)
    {
        this->reject();
        return;
    }

    // 保存修改
    for(int i = 0; i < _shortcut_list->count(); i++)
    {
        auto item = _shortcut_list->item(i);

        auto row = _shortcut_list->itemWidget(item);
        if(row == nullptr)
            continue;

        auto obj_name = item->data(Qt::UserRole).toString();
        if(obj_name.isEmpty())
            continue;

        auto edit = row->findChild<QKeySequenceEdit*>();
        if(edit == nullptr)
            continue;

        ReaderConfig::Instance()->setShortcut(obj_name, edit->keySequence().toString());
    }

    this->accept();
}
