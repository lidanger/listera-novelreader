#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bookbrowser.h"
#include "textcontent.h"
#include <DMainWindow>
#include <DDockWidget>
#include <DListView>
#include <QTimer>
#include <DTextEdit>
#include <QSystemTrayIcon>
#include <QTime>

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    void init_ui();
    void init_shortcuts();
    void init_data();
    void init_others();

private slots:
    void showDSettingsDialog();
    void dSettings_valueChanged(const QString &key, const QVariant &value);

    void library_clicked();
    void add_bookmark();

    void import_file();
    void import_directory();
    void remove_novel();

    void history_aboutToShow();
    void show_bookmarks();
    void view_contents();

    void booklist_currentRowChanged(int currentRow);

    void previous_page_pressed();
    void next_page_pressed();
    void next_screen_pressed();
    void current_page_edit_returnPressed();

    void shotcut_settings();

    void booklist_customContextMenuRequested(const QPoint &pos);

    void select_font();
    void select_text_color();
    void select_background_color();
    void show_dark_mode();
    void show_full_screen();
    void linespace_valueChanged(double value);
    void jumptoratio_returnPressed();
    void set_auto_scrolling();
    void autoscroll_timer_timeout();
    void autoscroll_wait_restart();
    void _find_text_returnPressed();

    void view_instructions();
    void send_feedback();

    void history_item_triggered();
    void clear_history();

    void toolbar_menu_aboutToShow();

    void web_search();
    void search_engine_select(QAction *action);

    void tray_icon_activated(QSystemTrayIcon::ActivationReason reason);

private:
    bool eventFilter(QObject *target, QEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    DToolBar *_toolbar;
    DDockWidget *_ldock;
    DListWidget *_booklist;
    TextContent *_content;
    QMenu *_content_menu;

    QLineEdit *_current_page_edit;
    QLineEdit *_total_pages_edit;

    QMenu *_reading_history_menu;
    QMenu *_booklist_menu;
    QMenu *_toolbar_menu;

    BookBrowser *_browser;

    QByteArray _state_before_fullscreen;
    Qt::WindowStates _window_state_before_fullscreen;

    QTimer *_autoscroll_timer;

    QSystemTrayIcon *_tray_icon;
    QDateTime _last_tray_active_time;
};

#endif // MAINWINDOW_H
