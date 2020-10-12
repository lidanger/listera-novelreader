#ifndef READINGHISTORY_H
#define READINGHISTORY_H

#include <DDialog>
#include <QTableView>
#include <QTime>

DWIDGET_USE_NAMESPACE

class Bookmarks : public DDialog
{
    Q_OBJECT

public:
    Bookmarks(QWidget *parent = nullptr);

private:
    void init_ui();
    void init_data();

private slots:
    void remove_triggered();
    void clear_triggered();
    void table_customContextMenuRequested(const QPoint &pos);
    void table_doubleClicked(const QModelIndex &index);

private:
    QTableView *_table;
    QMenu *_menu;

    QDateTime _last_click_time;
};

#endif // READINGHISTORY_H
