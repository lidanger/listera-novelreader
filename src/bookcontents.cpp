#include "bookcontents.h"

#include <QBoxLayout>
#include <QFile>
#include <QGroupBox>
#include <QListWidget>
#include <QTextBrowser>
#include <QTextCodec>

BookContents::BookContents(BookBrowser *browser, QWidget *parent)
    :DDialog (parent), _browser(browser)
{
    this->setFixedSize(800, 500);
    this->setWindowTitle(tr("View Book Contents"));

    auto panel = new QWidget(this);
    this->addContent(panel);

    auto panel_layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    panel->setLayout(panel_layout);

    auto toc_group = new QGroupBox(tr("Contents"), this);
    toc_group->setFixedWidth(380);
    panel_layout->addWidget(toc_group);

    auto toc_layout = new QBoxLayout(QBoxLayout::LeftToRight, toc_group);
    toc_group->setLayout(toc_layout);

    _toc_list = new QListWidget(toc_group);
    _toc_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    toc_layout->addWidget(_toc_list);
    connect(_toc_list, SIGNAL(currentRowChanged(int)), this, SLOT(toc_list_currentRowChanged(int)));
    connect(_toc_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(toc_list_itemDoubleClicked(QListWidgetItem *)));

    foreach(QString name, _browser->pageTitles())
    {
        auto item = new QListWidgetItem(name, _toc_list);
        item->setSizeHint(QSize(100, 30));
        _toc_list->addItem(item);
    }

    auto preview_group = new QGroupBox(tr("Preview"), this);
    preview_group->setFixedWidth(380);
    panel_layout->addWidget(preview_group);

    auto preview_layout = new QBoxLayout(QBoxLayout::LeftToRight, preview_group);
    preview_group->setLayout(preview_layout);

    _preview_text = new QTextBrowser(preview_group);
    preview_layout->addWidget(_preview_text);

    // 预览
    _toc_list->setCurrentRow(_browser->currentPage());
}

void BookContents::toc_list_currentRowChanged(int currentRow)
{
    auto item = _toc_list->item(currentRow);
    if(item == nullptr)
        return;

    auto txt = _browser->pagePreview(currentRow) + "......\n";
    _preview_text->setText(txt);
}

void BookContents::toc_list_itemDoubleClicked(QListWidgetItem *item)
{
    auto row = _toc_list->row(item);
    if(row != -1)
    {
        _browser->moveToPage(row);
    }

    this->accept();
}
