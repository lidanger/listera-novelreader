#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bookbrowser.h"
#include "textcontent.h"

#include <DMainWindow>
#include <DDockWidget>
#include <DListView>
#include <QTimer>

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    void make_ui();
    void init_ui();
    void init_shortcuts();
    void init_data();
    void init_others();

private slots:
    void library_clicked();
    void add_bookmark_clicked();

    void import_file();
    void import_directory();
    void remove_novel();

    void show_bookmarks();
    void view_contents_clicked();

    void booklist_currentRowChanged(int currentRow);

    void previous_page_pressed();
    void next_page_pressed();
    void next_screen_pressed();
    void current_page_edit_returnPressed();

    void view_shotcuts();

    void booklist_customContextMenuRequested(const QPoint &pos);

    void select_font();
    void select_font_color();
    void select_background_color();
    void show_dark_mode();
    void show_full_screen();
    void set_auto_scrolling();
    void autoscroll_timer_timeout();

    void view_instructions();
    void send_feedback();

private:
    bool eventFilter(QObject *target, QEvent *event);

    void closeEvent(QCloseEvent *event);

private:
    DDockWidget *_ldock;
    DListWidget *_booklist;
    TextContent *_content;
    DToolBar *_toolbar;

    QLineEdit *_current_page_edit;
    QLineEdit *_total_pages_edit;

    QMenu *_reading_history_menu;
    QMenu *_booklist_menu;

    BookBrowser *_browser;

    QByteArray _state_before_fullscreen;
    Qt::WindowStates _window_state_before_fullscreen;

    QTimer *_autoscroll_timer;
};

#endif // MAINWINDOW_H
