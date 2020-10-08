#ifndef BOOKCONTENTS_H
#define BOOKCONTENTS_H

#include "bookbrowser.h"

#include <DDialog>
#include <QListWidget>
#include <QTextBrowser>

DWIDGET_USE_NAMESPACE

class BookContents : public DDialog
{
    Q_OBJECT

public:
    BookContents(BookBrowser *browser, QWidget *parent = nullptr);

private slots:
    void toc_list_currentRowChanged(int currentRow);
    void toc_list_itemDoubleClicked(QListWidgetItem *item);

private:
    BookBrowser *_browser;

    QListWidget *_toc_list;
    QTextBrowser *_preview_text;
};

#endif // BOOKCONTENTS_H
