#include "bookmarks.h"
#include "readerconfig.h"
#include <QHeaderView>
#include <QMenu>
#include <QStandardItem>
#include <QDebug>
#include <QEvent>

Bookmarks::Bookmarks(QWidget *parent)
    : DDialog (parent)
{
    init_ui();
    init_data();
}

void Bookmarks::init_ui()
{
    this->setWindowTitle(tr("Bookmark List"));
    this->setFixedSize(700, 450);

    _table = new QTableView(this);
    this->addContent(_table);

    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_table, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(table_customContextMenuRequested(const QPoint &)));
    connect(_table, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(table_doubleClicked(const QModelIndex &)));

    _menu = new QMenu(_table);
    _menu->addAction(tr("Remove"), this, SLOT(remove_triggered()));
    _menu->addAction(tr("Clear"), this, SLOT(clear_triggered()));
}

void Bookmarks::init_data()
{
    auto keys = ReaderConfig::Instance()->bookmarks();
    if(keys.isEmpty())
        return;

    auto model = new QStandardItemModel();
    model->setColumnCount(3);
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Book Name")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Page Title")));
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Record Time")));
    _table->setModel(model);

    model->setRowCount(keys.size());

    for(int i = 0; i < keys.size(); i++)
    {
        auto key = keys[i];

        auto items = key.split('_', QString::SkipEmptyParts);
        if(items.length() != 3)
            continue;

        auto name = items[0];
        auto page_title = items[1];
        auto page = items[2];

        auto time = ReaderConfig::Instance()->bookmarkTime(key);
        if(time.length() == 14)
            time = QString("%1-%2-%3 %4:%5:%6")
                    .arg(time.left(4))
                    .arg(time.mid(4, 2))
                    .arg(time.mid(6, 2))
                    .arg(time.mid(8, 2))
                    .arg(time.mid(10, 2))
                    .arg(time.right(2));

        model->setItem(i, 0, new QStandardItem(name));
        model->setItem(i, 1, new QStandardItem(page_title));
        model->setItem(i, 2, new QStandardItem(time));

        model->item(i)->setData(key);
    }

    _table->setColumnWidth(0, 200);
    _table->setColumnWidth(1, 315);
    _table->setColumnWidth(2, 145);
}

void Bookmarks::remove_triggered()
{
    auto row = _table->selectionModel()->selectedRows().first().row();
    auto model = static_cast<QStandardItemModel*>(_table->model());
    auto key = model->item(row)->data().toString();

    ReaderConfig::Instance()->removeBookmark(key);
    model->removeRow(row);
}

void Bookmarks::clear_triggered()
{
    ReaderConfig::Instance()->clearBookmarks();
    _table->model()->removeRows(0, _table->model()->rowCount());
}

void Bookmarks::table_customContextMenuRequested(const QPoint &pos)
{
    if(_table->model() == nullptr || _table->model()->rowCount() == 0)
        return;

    auto row = _table->rowAt(pos.y());
    if(row == -1)
    {
        auto actions = _menu->findChildren<QAction*>();
        foreach(QAction *action, actions)
        {
            if(action->text() == tr("Remove"))
            {
                action->setVisible(false);
                break;
            }
        }
    }
    else
    {
        auto actions = _menu->findChildren<QAction*>();
        foreach(QAction *action, actions)
        {
            if(action->text() == tr("Remove"))
            {
                action->setVisible(true);
                break;
            }
        }
    }

    _menu->exec(QCursor::pos());
}

void Bookmarks::table_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    auto model = static_cast<QStandardItemModel*>(_table->model());
    auto key = model->item(index.row())->data().toString();

    auto items = key.split('_', QString::SkipEmptyParts);
    if(items.length() != 3)
        return;

    bool ok = false;
    auto page = items[2].toInt(&ok);
    if(!ok)
        return;

    ReaderConfig::Instance()->setCurrentBook(items[0]);
    ReaderConfig::Instance()->setBookCurrentPage(items[0], page);
    this->accept();
}
