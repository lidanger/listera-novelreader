#include "bookcontents.h"
#include "mainwindow.h"
#include "readerconfig.h"
#include "bookmarks.h"
#include "bookbrowser.h"
#include "shotcutsettings.h"
#include "utils.h"
#include <DTitlebar>
#include <QToolButton>
#include <QToolBar>
#include <QDockWidget>
#include <DStatusBar>
#include <DListWidget>
#include <QtDebug>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QResizeEvent>
#include <DFileDialog>
#include <DAboutDialog>
#include <DApplication>
#include <QTextBlock>
#include <QFontDialog>
#include <QColorDialog>
#include <QDesktopServices>
#include <QScrollBar>
#include <DSettings>
#include <DSettingsDialog>
#include <DSettingsGroup>
#include <DSettingsOption>
#include <QSpinBox>
#include <DToolButton>

DCORE_USE_NAMESPACE

MainWindow::MainWindow()
    : DMainWindow()
{
    _toolbar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, _toolbar);

    auto statusbar = new DStatusBar(this);
    this->setStatusBar(statusbar);

    _ldock = new QDockWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea, _ldock);

    _content = new TextContent(this);
    this->setCentralWidget(_content);

    _browser = new BookBrowser();

    init_ui();
    init_shortcuts();
    init_data();
    init_others();
}

void MainWindow::init_ui()
{
    // 窗口
    this->setMinimumSize(1000, 600);
    this->resize(ReaderConfig::Instance()->windowSize());
    this->setWindowState(static_cast<Qt::WindowState>(ReaderConfig::Instance()->windowState()));
    this->installEventFilter(this);
    this->setContextMenuPolicy(Qt::NoContextMenu);

    _toolbar_menu = new QMenu(this);
    connect(_toolbar_menu, SIGNAL(aboutToShow()), this, SLOT(toolbar_menu_aboutToShow()));

    auto action = _toolbar_menu->addAction(tr("ToolBar"), _toolbar, SLOT(setVisible(bool)));
    action->setCheckable(true);
    action->setChecked(ReaderConfig::Instance()->toolbarState());
    action->setObjectName("toolbar");
    action = _toolbar_menu->addAction(tr("StatusBar"), this->statusBar(), SLOT(setVisible(bool)));
    action->setCheckable(true);
    action->setChecked(ReaderConfig::Instance()->statusbarState());
    action->setObjectName("statusbar");
    _toolbar_menu->addSeparator();
    action = _toolbar_menu->addAction(tr("Library"), _ldock, SLOT(setVisible(bool)));
    action->setCheckable(true);
    action->setChecked(ReaderConfig::Instance()->sidebarState());
    action->setObjectName("library");

    // 关于
    auto about = new DAboutDialog(this);
    auto app = static_cast<DApplication*>(QApplication::instance());
    app->setAboutDialog(about);

    about->setVersion(app->applicationVersion());
    about->setCompanyLogo(QPixmap(":/images/logo_3.png").scaledToWidth(150));
    about->setDescription(app->applicationDescription());
    about->setProductIcon(app->productIcon());
    about->setProductName(app->productName());
    about->setWebsiteLink(app->applicationHomePage());
    about->setWebsiteName("www.listera.top");

    // 标题栏
    auto titlebar = this->titlebar();
    titlebar->setAutoHideOnFullscreen(true);
    titlebar->setIcon(app->productIcon());
    titlebar->setTitle(app->productName());

    auto button = new QToolButton(titlebar);
    titlebar->addWidget(button, Qt::AlignRight);
    button->setText(tr("View Contents"));
    button->setShortcut(Qt::Key_F6);
    connect(button, SIGNAL(clicked()), this, SLOT(view_contents()));
    button->setObjectName("viewcontents");

    button = new QToolButton(titlebar);
    titlebar->addWidget(button, Qt::AlignRight);
    button->setText(tr("Add BookMark"));
    button->setShortcut(Qt::Key_F3);
    connect(button, SIGNAL(clicked()), this, SLOT(add_bookmark()));
    button->setObjectName("addbookmark");

    button = new QToolButton(titlebar);
    titlebar->addWidget(button, Qt::AlignRight);
    button->setText(tr("Book Library"));
    button->setShortcut(Qt::Key_F2);
    connect(button, SIGNAL(clicked()), this, SLOT(library_clicked()));
    button->setObjectName("showlibrary");

    // 主菜单
    auto topmenu = new DMenu(titlebar);
    titlebar->setMenu(topmenu);

    _reading_history_menu = topmenu->addMenu(tr("Reading History"));
    connect(_reading_history_menu, SIGNAL(aboutToShow()), this, SLOT(history_aboutToShow()));
    topmenu->addAction(tr("Bookmarks"), this, SLOT(show_bookmarks()));
    topmenu->addSeparator();
    topmenu->addAction(tr("Shotcuts"), this, SLOT(shotcut_settings()));
    topmenu->addSeparator();
    topmenu->addAction(tr("Instructions"), this, SLOT(view_instructions()));
    topmenu->addAction(tr("Feedback"), this, SLOT(send_feedback()));
    topmenu->addSeparator();

    // 工具栏
    _toolbar->setWindowTitle(tr("ToolBar"));
    _toolbar->setStyleSheet(QString("QToolBar{border:none;}"));
    _toolbar->setVisible(ReaderConfig::Instance()->toolbarState());
    _toolbar->installEventFilter(this);

    _toolbar->addAction(tr("Font"), this, SLOT(select_font()))->setObjectName("font");

    _toolbar->addAction(tr("Text Color"), this, SLOT(select_text_color()))->setObjectName("textcolor");

    _toolbar->addSeparator();

    _toolbar->addAction(tr("Background Color"), this, SLOT(select_background_color()))->setObjectName("backgroundcolor");

    action = _toolbar->addAction(tr("Dark Mode"), this, SLOT(show_dark_mode()));
    action->setCheckable(true);
    action->setChecked(ReaderConfig::Instance()->darkMode());
    action->setObjectName("darkmode");
    action->setShortcut(Qt::Key_F10);
    this->addAction(action);

    _toolbar->addSeparator();

    _toolbar->addWidget(new QLabel(tr("Line Space"), _toolbar));
    auto spin = new QDoubleSpinBox(_toolbar);
    _toolbar->addWidget(spin);
    spin->setFixedWidth(90);
    spin->setMinimum(1);
    spin->setMaximum(3);
    spin->setDecimals(2);
    spin->setSingleStep(0.05);
    spin->setValue(ReaderConfig::Instance()->lineSpace());
    connect(spin, SIGNAL(valueChanged(double)), this, SLOT(linespace_valueChanged(double)));

    _toolbar->addSeparator();

    action = _toolbar->addAction(tr("Full Screen"), this, SLOT(show_full_screen()));
    action->setCheckable(true);
    action->setShortcut(Qt::Key_F11);
    action->setObjectName("fullscreen");
    this->addAction(action);

    _toolbar->addSeparator();

    action = _toolbar->addAction(tr("Auto Scrolling"), this, SLOT(set_auto_scrolling()));
    action->setCheckable(true);
    action->setObjectName("autoscroll");
    action->setShortcut(QKeySequence(Qt::Key_ScrollLock));
    this->addAction(action);

    auto edit = new QLineEdit("500", _toolbar);
    _toolbar->addWidget(edit);
    edit->setObjectName("autoscroll_interval");
    edit->setFixedWidth(50);
    _toolbar->addWidget(new QLabel("ms", _toolbar));

    _toolbar->addSeparator();

    _toolbar->addWidget(new QLabel(tr("Jump to Ratio"), _toolbar));
    edit = new QLineEdit(_toolbar);
    _toolbar->addWidget(edit);
    edit->setObjectName("jumptoratio");
    edit->setPlaceholderText(tr("Ratio"));
    edit->setFixedWidth(50);
    connect(edit, SIGNAL(returnPressed()), this, SLOT(jumptoratio_returnPressed()));
    _toolbar->addWidget(new QLabel("%", _toolbar));

    _toolbar->addSeparator();

    edit = new QLineEdit(_toolbar);
    _toolbar->addWidget(edit);
    edit->setObjectName("texttofind");
    edit->setPlaceholderText(tr("Input to find"));
    edit->setFixedWidth(100);
    connect(edit, SIGNAL(returnPressed()), this, SLOT(_find_text_returnPressed()));
    action = _toolbar->addAction(tr("Find text"), this, SLOT(_find_text_returnPressed()));
    action->setShortcut(QKeySequence(QKeySequence::Find));
    action->setObjectName("findtext");
    this->addAction(action);

    _toolbar->addSeparator();

    edit = new QLineEdit(_toolbar);
    _toolbar->addWidget(edit);
    edit->setPlaceholderText(tr("Input to search"));
    edit->setFixedWidth(150);
    edit->setObjectName("searchtext");
    connect(edit, SIGNAL(returnPressed()), this, SLOT(web_search()));
    button = new DToolButton(_toolbar);
    _toolbar->addWidget(button);
    action = new QAction(tr("Search"), _toolbar);
    action->setObjectName("search");
    connect(action, SIGNAL(triggered()), this, SLOT(web_search()));
    button->setDefaultAction(action);
    button->setPopupMode(QToolButton::DelayedPopup);
    auto menu = new QMenu(button);
    button->setMenu(menu);

    auto current_engine = ReaderConfig::Instance()->searchEngine();
    if(!current_engine.isEmpty())
        edit->setPlaceholderText(current_engine);

    auto group = new QActionGroup(this);
    group->setExclusive(true);
    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(search_engine_select(QAction*)));
    auto engines = searchEngines();
    foreach(QString engine, engines)
    {
        auto action = menu->addAction(engine);
        action->setCheckable(true);
        group->addAction(action);

        if(!current_engine.isEmpty() && current_engine == engine)
        {
            action->setChecked(true);
        }
    }

    // 不显示，用于定义快捷键
    action = _toolbar->addAction(tr("Boss Key"));
    action->setVisible(false);
    action->setObjectName("bosskey");
    // 另创建一个用于使用
    action = new QAction(this);
    connect(action, &QAction::triggered, this, &MainWindow::hide);
    action->setObjectName("bosskey_real");
    action->setShortcut(QKeySequence("Alt+Z"));
    this->addAction(action);

    // 状态栏
    this->statusBar()->setFixedHeight(30);
    this->statusBar()->setStyleSheet(QString( "QStatusBar::item{border: 0px}"));
    this->statusBar()->setVisible(ReaderConfig::Instance()->statusbarState());
    this->statusBar()->installEventFilter(this);

    auto total_label = new QLabel(this->statusBar());
    total_label->setText(tr("Total Pages"));
    this->statusBar()->addPermanentWidget(total_label);

    _total_pages_edit = new QLineEdit(this->statusBar());
    this->statusBar()->addPermanentWidget(_total_pages_edit);
    _total_pages_edit->setFixedWidth(50);
    _total_pages_edit->setReadOnly(true);
    _total_pages_edit->setFocusPolicy(Qt::NoFocus);

    auto current_label = new QLabel(this->statusBar());
    current_label->setText(tr("Current Page"));
    this->statusBar()->addPermanentWidget(current_label);

    _current_page_edit = new QLineEdit(this->statusBar());
    this->statusBar()->addPermanentWidget(_current_page_edit);
    _current_page_edit->setFixedWidth(50);
    connect(_current_page_edit, SIGNAL(returnPressed()), this, SLOT(current_page_edit_returnPressed()));

    button = new DToolButton(this->statusBar());
    this->statusBar()->addPermanentWidget(button);
    action = new QAction(tr("Previous Page"), button);
    button->setDefaultAction(action);
    this->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(previous_page_pressed()));
    action->setShortcut(Qt::Key_Left);
    action->setObjectName("prevpage");

    button = new DToolButton(this->statusBar());
    this->statusBar()->addPermanentWidget(button);
    action = new QAction(tr("Next Screen"), button);
    button->setDefaultAction(action);
    this->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(next_screen_pressed()));
    action->setShortcut(Qt::Key_Space);
    action->setObjectName("nextscreen");

    button = new DToolButton(this);
    this->statusBar()->addPermanentWidget(button);
    action = new QAction(tr("Next Page"), button);
    button->setDefaultAction(action);
    this->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(next_page_pressed()));
    action->setShortcut(Qt::Key_Right);
    action->setObjectName("nextpage");

    // 侧栏
    _ldock->setWindowTitle(tr("Library"));
    _ldock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
    //_ldock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    _ldock->setMinimumWidth(150);
    _ldock->setMaximumWidth(500);
    auto width = ReaderConfig::Instance()->sidebarWidth();
    if(width == 0)
        width = _ldock->minimumWidth();
    this->resizeDocks({_ldock}, {width}, Qt::Horizontal);
    _ldock->setVisible(ReaderConfig::Instance()->sidebarState());
    _ldock->installEventFilter(this);
    _ldock->setContextMenuPolicy(Qt::CustomContextMenu);

    _booklist = new DListWidget(_ldock);
    _ldock->setWidget(_booklist);

    connect(_booklist, SIGNAL(currentRowChanged(int)), this, SLOT(booklist_currentRowChanged(int)));
    _booklist->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_booklist, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(booklist_customContextMenuRequested(const QPoint &)));

    _booklist_menu = new QMenu(_booklist);
    _booklist_menu->addAction(tr("Import File..."), this, SLOT(import_file()));
    _booklist_menu->addAction(tr("Import Directory..."), this, SLOT(import_directory()));
    _booklist_menu->addSeparator();
    _booklist_menu->addAction(tr("Remove"), this, SLOT(remove_novel()));

    // 内容区
    _content->setFontFamily(ReaderConfig::Instance()->fontFamily());
    _content->setFontPointSize(ReaderConfig::Instance()->fontSize());
    _content->setTextColor(ReaderConfig::Instance()->textColor());
    _content->setBackgroundColor(ReaderConfig::Instance()->backgroundColor());
    _content->setLineSpacing(ReaderConfig::Instance()->lineSpace());
    _content->setCharSpacing(ReaderConfig::Instance()->characterSpace());
    _content->setParagraphSpaceing(ReaderConfig::Instance()->paragraphSpace());
    _content->setRemoveEmptyLine(ReaderConfig::Instance()->removeEmptyLine());
    _content->setDarkMode(ReaderConfig::Instance()->darkMode());

    // 通知栏图标
    _tray_icon = new QSystemTrayIcon(app->productIcon(), this);
    connect(_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(tray_icon_activated(QSystemTrayIcon::ActivationReason)));
    _tray_icon->setToolTip(app->productName());
    _tray_icon->show();
}

void MainWindow::init_shortcuts()
{
    // 标题栏按钮
    auto buttons = this->titlebar()->findChildren<QToolButton*>();
    foreach(QToolButton *button, buttons)
    {
        auto shortcut = ReaderConfig::Instance()->shortcutKeys(button->objectName());
        if(shortcut.isEmpty())
            continue;

        button->setShortcut(shortcut);
    }

    // 工具栏
    buttons = _toolbar->findChildren<QToolButton*>();
    foreach(QToolButton *button, buttons)
    {
        auto action = button->defaultAction();
        if(action == nullptr)
            continue;

        auto shortcut = ReaderConfig::Instance()->shortcutKeys(action->objectName());
        if(shortcut.isEmpty())
            continue;

        if(action->objectName() == "bosskey")
            this->findChild<QAction*>("bosskey_real")->setShortcut(shortcut);
        else
            action->setShortcut(shortcut);
    }

    // 状态栏
     buttons = this->statusBar()->findChildren<QToolButton*>();
    foreach(QToolButton *button, buttons)
    {
        auto action = button->defaultAction();

        auto shortcut = ReaderConfig::Instance()->shortcutKeys(action->objectName());
        if(shortcut.isEmpty())
            continue;

        action->setShortcut(shortcut);
    }
}

void MainWindow::init_data()
{
    // 书库
    auto books = ReaderConfig::Instance()->bookNames();
    if(!books.isEmpty())
    {
        auto current_book = ReaderConfig::Instance()->currentBook();

        for(auto it = books.begin(); it != books.end(); it++)
        {
            auto path = ReaderConfig::Instance()->bookPath(*it);
            if(path.isEmpty())
                continue;

            auto item = new QListWidgetItem(*it, _booklist);
            item->setData(Qt::UserRole, path);
            item->setSizeHint(QSize(100, 30));
            _booklist->addItem(item);
        }

        if(!current_book.isEmpty())
        {
            auto lst = _booklist->findItems(current_book, Qt::MatchExactly);
            if(!lst.isEmpty())
                _booklist->setCurrentItem(lst.first());
        }
    }
}

void MainWindow::init_others()
{
    _autoscroll_timer = new QTimer(this);
    connect(_autoscroll_timer, SIGNAL(timeout()), this, SLOT(autoscroll_timer_timeout()));
}

void MainWindow::showDSettingsDialog()
{
    auto json = ReaderConfig::Instance()->getDSettingsJson();
    auto sts = DSettings::fromJson(json);
    connect(sts, SIGNAL(valueChanged(const QString &, const QVariant &)), this, SLOT(dSettings_valueChanged(const QString &, const QVariant &)));

    DSettingsDialog dlg(this);
    dlg.updateSettings(sts);
    dlg.exec();
}

void MainWindow::dSettings_valueChanged(const QString &key, const QVariant &value)
{
    // basic.Reading.放开那个女巫"
    auto names = key.split('.', QString::SkipEmptyParts);
    if(names.length() != 3)
        return;

    ReaderConfig::Instance()->setValue(names[1], names[2], value);
}

void MainWindow::library_clicked()
{
    _ldock->setVisible(!_ldock->isVisible());
}

void MainWindow::import_file()
{
    auto lst = DFileDialog::getOpenFileNames(this, tr("Select Files"), ".", "Text Files (*.txt)");
    if(lst.empty())
        return;

    for(auto it = lst.begin(); it != lst.end(); it++)
    {
        QFileInfo fi(*it);

        auto item = new QListWidgetItem(fi.baseName(), _booklist);
        item->setData(Qt::UserRole, *it);
        item->setSizeHint(QSize(100, 30));
        _booklist->addItem(item);

        ReaderConfig::Instance()->addBook(*it);
    }

    if(_booklist->count() > 0)
        _booklist->setCurrentRow(_booklist->count() - 1);
}

void MainWindow::import_directory()
{
    auto dirpath = DFileDialog::getExistingDirectory(this, tr("Select Directory"), ".");
    if(dirpath.isEmpty())
        return;

    auto dir = QDir(dirpath);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QString("*.txt;").split(";"));
    auto lst = dir.entryInfoList();

    for(auto it = lst.begin(); it != lst.end(); it++)
    {
        auto item = new QListWidgetItem(it->baseName(), _booklist);
        item->setData(Qt::UserRole, it->absoluteFilePath());
        item->setSizeHint(QSize(100, 30));
        _booklist->addItem(item);

        ReaderConfig::Instance()->addBook(it->absoluteFilePath());
    }

    if(_booklist->count() > 0)
        _booklist->setCurrentRow(_booklist->count() - 1);
}

void MainWindow::remove_novel()
{
    auto item = _booklist->currentItem();
    if(item == nullptr)
        return;

    if(QMessageBox::question(this, tr("Remove Confirm"), tr("You are going to remove this item, continue?")) != QMessageBox::Yes)
        return;

    ReaderConfig::Instance()->removeBook(item->text());

    auto row = _booklist->currentRow();
    if(row > 0)
        _booklist->setCurrentRow(row - 1);
    else
    {
        if(_booklist->count() > 1)
            _booklist->setCurrentRow(1);
    }

    item = _booklist->takeItem(row);
    if(item != nullptr)
        delete item;
}

void MainWindow::history_aboutToShow()
{
    _reading_history_menu->clear();

    auto items = ReaderConfig::Instance()->readingHistory();
    if(items.isEmpty())
        return;

    auto name = ReaderConfig::Instance()->readingHistoryItem(items.last());
    while(name == _browser->name())
    {
        items.pop_back();

        if(items.isEmpty())
            break;

        name = ReaderConfig::Instance()->readingHistoryItem(items.last());
    }

    if(items.isEmpty())
        return;

    int count = 0;
    QString current;
    foreach(QString item, items)
    {
        name = ReaderConfig::Instance()->readingHistoryItem(item);
        if(current == name)
            continue;

        current = name;

        _reading_history_menu->addAction(name, this, SLOT(history_item_triggered()))->setCheckable(true);

        ++count;
        if(count == 10)
            break;
    }

    _reading_history_menu->addSeparator();
    _reading_history_menu->addAction(tr("Clear"), this, SLOT(clear_history()));
}

void MainWindow::show_bookmarks()
{
    Bookmarks dlg(this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    if(ReaderConfig::Instance()->currentBook() == _browser->name())
    {
        if(!_browser->moveToPage(ReaderConfig::Instance()->bookCurrentPage(_browser->name())))
        {
            if(!_browser->fileExists())
                QMessageBox::information(this, tr("File not found"), tr("The novel file can not be found."), QMessageBox::Ok);
            return;
        }
        _content->setText(_browser->currentPageContent());
        _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
    }
    else
    {
        auto items = _booklist->findItems(ReaderConfig::Instance()->currentBook(), Qt::MatchExactly);
        if(items.empty())
            return;

        _booklist->setCurrentItem(items.first());
    }
}

void MainWindow::view_contents()
{
    if(_booklist->currentRow() == -1)
        return;

    BookContents dlg(_browser, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    _content->setText(_browser->currentPageContent());
    _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
}

void MainWindow::add_bookmark()
{
    if(_booklist->currentRow() == -1)
        return;

    ReaderConfig::Instance()->addBookmark(_browser->name(), _browser->currentPage(), _browser->currentPageTitle());
}

void MainWindow::booklist_currentRowChanged(int currentRow)
{
    if(currentRow == -1)
        return;

    // 获取文件
    auto item = _booklist->item(currentRow);
    auto _current_book = item->text();
    auto filepath = item->data(Qt::UserRole).toString();

    ReaderConfig::Instance()->setCurrentBook(_current_book);

    if(_browser != nullptr)
        delete _browser;
    _browser = new BookBrowser();

    this->titlebar()->setTitle(_browser->name());

    if(!_browser->isAvailable())
    {
        if(_browser->fileExists())
            QMessageBox::information(this, tr("Unsupported text encoding"), QString(tr("Text containing in the following file can not be recognized correctly:\n%1")).arg(filepath), QMessageBox::Ok);
        else
            QMessageBox::information(this, tr("File not found"), QString(tr("The following file can not be found:\n%1")).arg(filepath), QMessageBox::Ok);

        return;
    }

    // 加载页
    _content->setText(_browser->currentPageContent());
    _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
    _total_pages_edit->setText(QString("%1").arg(_browser->pageNumber()));
}

void MainWindow::previous_page_pressed()
{
    if(_booklist->currentRow() == -1)
        return;

    if(!_browser->movePrevious())
    {
        if(!_browser->fileExists())
            QMessageBox::information(this, tr("File not found"), tr("The novel file can not be found."), QMessageBox::Ok);
        return;
    }

    _content->setText(_browser->currentPageContent());
    _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
}

void MainWindow::next_page_pressed()
{
    if(_booklist->currentRow() == -1)
        return;

    if(!_browser->moveNext())
    {
        if(!_browser->fileExists())
            QMessageBox::information(this, tr("File not found"), tr("The novel file can not be found."), QMessageBox::Ok);
        return;
    }

    _content->setText(_browser->currentPageContent());
    _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
}

void MainWindow::next_screen_pressed()
{
    if(_content->textCursor().atEnd())
    {
        next_page_pressed();
        return;
    }

    int y = _content->height() * 2 - 60;
    auto cursor = _content->cursorForPosition(QPoint(0, y));
    _content->setTextCursor(cursor);
}

void MainWindow::current_page_edit_returnPressed()
{
    this->setFocus();

    if(_booklist->currentRow() == -1)
        return;

    auto txt = _current_page_edit->text().trimmed();
    bool ok = false;
    auto page_index = txt.toInt(&ok);
    if(!ok)
        return;

    if(!_browser->moveToPage(page_index - 1))
    {
        if(!_browser->fileExists())
            QMessageBox::information(this, tr("File not found"), tr("The novel file can not be found."), QMessageBox::Ok);

        _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
        return;
    }

    _content->setText(_browser->currentPageContent());
}

void MainWindow::shotcut_settings()
{
    ShotcutSettings dlg(this);
    if(dlg.exec() == 0)
        init_shortcuts();
}

void MainWindow::booklist_customContextMenuRequested(const QPoint &pos)
{
    auto actions = _booklist_menu->findChildren<QAction*>();
    if(!actions.isEmpty())
    {
        foreach(QAction* action, actions)
        {
            if(action->text() == tr("Remove"))
            {
                auto item = _booklist->itemAt(pos);
                if(item == nullptr)
                    action->setVisible(false);
                else
                    action->setVisible(true);

                break;
            }
        }
    }

    _booklist_menu->exec(QCursor::pos());
}

void MainWindow::select_font()
{
    bool ok = false;
    auto font = QFontDialog::getFont(&ok, _content->font(), this);
    if(!ok)
        return;

    _content->setFont(font);

    ReaderConfig::Instance()->setFontFamily(font.family());
    ReaderConfig::Instance()->setFontSize(font.pointSizeF());
}

void MainWindow::select_text_color()
{
    auto color = QColorDialog::getColor(_content->textColor(), this);
    if(!color.isValid())
        return;

    _content->setTextColor(color);

    ReaderConfig::Instance()->setTextColor(color);
}

void MainWindow::select_background_color()
{
    auto color = QColorDialog::getColor(_content->backgroundColor(), this);
    if(!color.isValid())
        return;

    _content->setBackgroundColor(color);

    ReaderConfig::Instance()->setBackgroundColor(color);
}

void MainWindow::show_dark_mode()
{
    auto action = _toolbar->findChild<QAction*>("darkmode");
    if(action == nullptr)
        return;

    _content->setDarkMode(action->isChecked());

    ReaderConfig::Instance()->setDarkMode(action->isChecked());
}

void MainWindow::show_full_screen()
{
    if(this->isFullScreen())
    {
        this->showNormal();

        this->restoreState(_state_before_fullscreen);
        this->setWindowState(_window_state_before_fullscreen);

        _toolbar->setVisible(_toolbar_menu->findChild<QAction*>("toolbar")->isChecked());
        this->statusBar()->setVisible(_toolbar_menu->findChild<QAction*>("statusbar")->isChecked());
        _ldock->setVisible(_toolbar_menu->findChild<QAction*>("library")->isChecked());
    }
    else
    {
        _state_before_fullscreen = this->saveState();
        _window_state_before_fullscreen = this->windowState();

        this->showFullScreen();

        _ldock->hide();
        _toolbar->hide();
        this->statusBar()->hide();
    }

    this->setFocus();
}

void MainWindow::set_auto_scrolling()
{
    if(_content->text().isNull())
        return;

    auto action = _toolbar->findChild<QAction*>("autoscroll");

    if(action->isChecked())
    {
        auto edit = _toolbar->findChild<QLineEdit*>("autoscroll_interval");
        auto txt = edit->text().trimmed();

        bool ok = false;
        auto interval = txt.toInt(&ok);
        if(ok)
        {
            if(interval < 100)
                interval = 100;
        }
        else
        {
            edit->setText("500");
            interval = 500;
        }

        auto cursor = _content->cursorForPosition(QPoint(0, _content->height()));
        _content->setTextCursor(cursor);

        _autoscroll_timer->start(interval);
    }
    else
        _autoscroll_timer->stop();
}

void MainWindow::autoscroll_timer_timeout()
{
    if(_content->textCursor().atEnd())
    {
        _autoscroll_timer->stop();
        QTimer::singleShot(1000, this, SLOT(autoscroll_wait_restart()));
        return;
    }
    else if(_content->textCursor().atStart())
    {
        _autoscroll_timer->stop();
        QTimer::singleShot(2000, this, SLOT(autoscroll_wait_restart()));
        return;
    }

    _content->moveCursor(QTextCursor::Down);
}

void MainWindow::autoscroll_wait_restart()
{
    auto action = _toolbar->findChild<QAction*>("autoscroll");
    if(!action->isChecked())
        return;

    if(_content->textCursor().atEnd())
    {
        next_page_pressed();
    }
    else if(_content->textCursor().atStart())
    {
        auto cursor = _content->cursorForPosition(QPoint(0, _content->height()));
        _content->setTextCursor(cursor);
    }

    if(action->isChecked())
        _autoscroll_timer->start();
}

void MainWindow::_find_text_returnPressed()
{
    this->setFocus();

    _toolbar->show();

    auto edit = _toolbar->findChild<QLineEdit*>("texttofind");
    edit->setFocus();

    auto txt = edit->text().trimmed();
    if(!txt.isEmpty())
        _content->find(txt);
}

void MainWindow::view_instructions()
{
    QDesktopServices::openUrl(QUrl(tr("https://www.listera.top/tag/novelreader/")));
}

void MainWindow::send_feedback()
{
    QDesktopServices::openUrl(QUrl("https://www.listera.top/tag/novelreader/"));
}

void MainWindow::history_item_triggered()
{
    auto actions = _reading_history_menu->findChildren<QAction*>();
    foreach(QAction *action, actions)
    {
        if(action->isChecked())
        {
            auto items = _booklist->findItems(action->text(), Qt::MatchFlag::MatchExactly);
            if(!items.isEmpty())
            {
                _booklist->setCurrentItem(items.first());
            }

            action->setChecked(false);
            break;
        }
    }
}

void MainWindow::clear_history()
{
    if(QMessageBox::question(this, tr("Confirm"), tr("You are going to clear the reading history, continue?")) != QMessageBox::Yes)
        return;

    ReaderConfig::Instance()->clearHistory();
}

void MainWindow::toolbar_menu_aboutToShow()
{
    _toolbar_menu->findChild<QAction*>("toolbar")->setChecked(_toolbar->isVisible());
    _toolbar_menu->findChild<QAction*>("statusbar")->setChecked(this->statusBar()->isVisible());
    _toolbar_menu->findChild<QAction*>("library")->setChecked(_ldock->isVisible());
}

void MainWindow::linespace_valueChanged(double value)
{
    _content->setLineSpacing(value);

    ReaderConfig::Instance()->setLineSpace(value);
}

void MainWindow::jumptoratio_returnPressed()
{
    this->setFocus();

    auto edit = _toolbar->findChild<QLineEdit*>("jumptoratio");

    auto txt = edit->text().trimmed();
    bool ok = false;
    auto ratio = txt.toFloat(&ok);
    if(ok)
    {
        if(!_browser->moveToRatio(ratio / 100))
        {
            if(!_browser->fileExists())
                QMessageBox::information(this, tr("File not found"), tr("The novel file can not be found."), QMessageBox::Ok);
            return;
        }

        _content->setText(_browser->currentPageContent());
        _current_page_edit->setText(QString("%1").arg(_browser->currentPage()));
    }
}

void MainWindow::web_search()
{
    auto edit = _toolbar->findChild<QLineEdit*>("searchtext");
    auto txt = edit->text().trimmed();
    if(txt.isEmpty())
        return;

    auto engine = ReaderConfig::Instance()->searchEngine();
    startWebSearch(txt, engine);
}

void MainWindow::search_engine_select(QAction *action)
{
    auto edit = _toolbar->findChild<QLineEdit*>("searchtext");
    edit->setPlaceholderText(action->text());

    ReaderConfig::Instance()->setSearchEngine(action->text());
}

void MainWindow::tray_icon_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        if(_last_tray_active_time.addMSecs(300) > QDateTime::currentDateTime())
            tray_icon_activated(QSystemTrayIcon::DoubleClick);
        _last_tray_active_time = QDateTime::currentDateTime();
        break;
    case QSystemTrayIcon::DoubleClick:
        this->setVisible(!this->isVisible());
        break;
    case QSystemTrayIcon::Context:
        break;
    default:
        break;
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        auto keve = static_cast<QKeyEvent*>(event);

        if(keve->key() == Qt::Key_Escape)
        {
            //show_full_screen();
            this->setFocus();
        }
        else if(keve->key() == Qt::Key_Return || keve->key() == Qt::Key_Enter)
        {

        }
        else if(event->type() == QEvent::KeyPress)
        {
            auto keve = static_cast<QKeyEvent*>(event);

            if(target == this)
            {
                if(keve->key() == Qt::Key_Up)
                {
                    auto cursor = _content->cursorForPosition(QPoint(0, 0));
                    _content->setTextCursor(cursor);
                    _content->moveCursor(QTextCursor::Up);
                    _content->moveCursor(QTextCursor::Up);
                    _content->moveCursor(QTextCursor::Up);
                    _content->moveCursor(QTextCursor::Up);
                    _content->moveCursor(QTextCursor::Up);
                    _content->moveCursor(QTextCursor::Up);
                }
                else if(keve->key() == Qt::Key_Down)
                {
                    auto cursor = _content->cursorForPosition(QPoint(0, this->height()));
                    _content->setTextCursor(cursor);
                    _content->moveCursor(QTextCursor::Down);
                    _content->moveCursor(QTextCursor::Down);
                }
            }
        }
    }
    else if(event->type() == QEvent::ContextMenu)
    {
        if(target == _ldock || target == _toolbar || target == this->statusBar())
        {
            _toolbar_menu->exec(QCursor::pos());
        }
    }

    // 处理其他消息
    return DMainWindow::eventFilter(target, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    // 保存配置
    ReaderConfig::Instance()->setWindowSize(this->size());
    ReaderConfig::Instance()->setWindowState(static_cast<int>(this->windowState()));
    ReaderConfig::Instance()->setSidebarState(_ldock->isVisible());
    ReaderConfig::Instance()->setSidebarWidth(_ldock->width());
    ReaderConfig::Instance()->setToolbarState(_toolbar->isVisible());
    ReaderConfig::Instance()->setStatusbarState(this->statusBar()->isVisible());

    //this->hide();
    //event->ignore();
}
