#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "textcontent.h"

#include <DMainWindow>
#include <DDockWidget>
#include <DListView>

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void on_sidebar_clicked();

private:
    void init_ui();
    //  初始化快捷键操作
    void init_shortcuts();

    bool eventFilter(QObject *target, QEvent *event);

private:
    DDockWidget *_ldock;
    DListWidget *_booklist;
    TextContent *_content;
};

#endif // MAINWINDOW_H
