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

    void import_file();
    void import_directory();

    void show_history();
    void view_book_contents();
    void show_advanced_options();
    void show_jump_ratio();

    void booklist_currentRowChanged(int currentRow);

    void previous_page_pressed();
    void next_page_pressed();
    void next_screen_pressed();
    void current_page_edit_returnPressed();

private:
    void init_ui();

    void init_shortcuts();

    void displayShortcuts();

    bool eventFilter(QObject *target, QEvent *event);

    void init_data();

private:
    DDockWidget *_ldock;
    DListWidget *_booklist;
    TextContent *_content;

    QLineEdit *_current_page_edit;
    QLineEdit *_total_pages_edit;

    QTextCodec *_codec;
    QMap<QString, qint64> _toc_indexs;
    QList<QString> _toc_names;
    int _current_page;
    QString _current_book;

    // action 名称->hotkeys
    QMap<QString, QString> _hotkeys;

private:
    const QString hotkey_next_page = "Right";
    const QString hotkey_previous_page = "Left";
    const QString hotkey_next_screen = "PgDown";
    const QString hotkey_previous_screen = "PgUp";
    const QString hotkey_next_screen2 = "Space";
};

#endif // MAINWINDOW_H
