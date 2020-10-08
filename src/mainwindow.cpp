#include "bookcontents.h"
#include "mainwindow.h"
#include "readerconfig.h"
#include "bookmarks.h"
#include "bookbrowser.h"
#include "readersettings.h"
#include "utils.h"
#include <DTitlebar>
#include <DToolButton>
#include <DToolBar>
#include <QDockWidget>
#include <DStatusBar>
#include <QSignalMapper>
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

MainWindow::MainWindow()
    : DMainWindow()
{
    make_ui();
    init_ui();
    init_shortcuts();
    init_data();
    init_others();
}

void MainWindow::make_ui()
{
    // 窗口
    this->setMinimumSize(1000, 600);
    this->installEventFilter(this);

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
    titlebar->setObjectName("titlebar");
    titlebar->setAutoHideOnFullscreen(true);
    titlebar->setIcon(app->productIcon());
    titlebar->setTitle(app->productName());

    auto toc = new DToolButton(titlebar);
    toc->setText(tr("View Contents"));
    titlebar->addWidget(toc, Qt::AlignRight);
    connect(toc, SIGNAL(clicked()), this, SLOT(view_contents_clicked()));

    auto mark = new DToolButton(titlebar);
    mark->setText(tr("Add BookMark"));
    titlebar->addWidget(mark, Qt::AlignRight);
    connect(mark, SIGNAL(clicked()), this, SLOT(add_bookmark_clicked()));

    auto sidebar = new DToolButton(titlebar);
    sidebar->setText(tr("Book Library"));
    titlebar->addWidget(sidebar, Qt::AlignRight);
    connect(sidebar, SIGNAL(clicked()), this, SLOT(library_clicked()));

    // 主菜单
    auto topmenu = new DMenu(titlebar);
    titlebar->setMenu(topmenu);

    _reading_history_menu = topmenu->addMenu(tr("Reading History"));
    topmenu->addAction(tr("Bookmarks"), this, SLOT(show_bookmarks()));
    topmenu->addSeparator();
    topmenu->addAction(tr("Instructions"), this, SLOT(view_instructions()));
    topmenu->addAction(tr("Feedback"), this, SLOT(send_feedback()));
    topmenu->addSeparator();
    topmenu->addAction(tr("Settings"), this, SLOT(view_shotcuts()));
    topmenu->addSeparator();

    // 工具栏
    _toolbar = new DToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, _toolbar);

    _toolbar->setObjectName("toolbar");
    _toolbar->setWindowTitle(tr("Toolbar"));
    _toolbar->hide();

    _toolbar->addAction(tr("Font"), this, SLOT(select_font()));
    _toolbar->addAction(tr("Font Color"), this, SLOT(select_font_color()));
    _toolbar->addAction(tr("Background Color"), this, SLOT(select_background_color()));
    _toolbar->addAction(tr("Dark Mode"), this, SLOT(show_dark_mode()));

    _toolbar->addWidget(new QLabel(tr("Line Space:"), _toolbar));

    auto linespace = new QLineEdit(_toolbar);
    _toolbar->addWidget(linespace);

    linespace->setObjectName("linespace");
    linespace->setPlaceholderText(tr("Line Space"));
    linespace->setFixedWidth(60);
    linespace->installEventFilter(this);

    _toolbar->addSeparator();
    _toolbar->addAction(tr("Full Screen"), this, SLOT(show_full_screen()));
    _toolbar->addSeparator();
    _toolbar->addAction(tr("Auto Scrolling"), this, SLOT(set_auto_scrolling()))->setObjectName("set_auto_scrolling");
    _toolbar->addSeparator();

    _toolbar->addWidget(new QLabel(tr("Jump to Ratio:"), _toolbar));

    auto jumptoratio = new QLineEdit(_toolbar);
    _toolbar->addWidget(jumptoratio);

    jumptoratio->setObjectName("jumptoratio");
    jumptoratio->setPlaceholderText(tr("Ratio"));
    jumptoratio->setFixedWidth(60);
    jumptoratio->installEventFilter(this);

    _toolbar->addWidget(new QLabel("%", _toolbar));

    // 状态栏
    auto statusbar = new DStatusBar(this);
    this->setStatusBar(statusbar);

    statusbar->setObjectName("statusbar");
    statusbar->setFixedHeight(30);
    statusBar()->setStyleSheet(QString( "QStatusBar::item{border: 0px}"));

    auto total_label = new QLabel(statusbar);
    total_label->setText(tr("Total Pages"));
    statusbar->addPermanentWidget(total_label);

    _total_pages_edit = new QLineEdit(statusbar);
    statusbar->addPermanentWidget(_total_pages_edit);
    _total_pages_edit->setFixedWidth(50);
    _total_pages_edit->setReadOnly(true);
    _total_pages_edit->setFocusPolicy(Qt::NoFocus);

    auto current_label = new QLabel(statusbar);
    current_label->setText(tr("Current Page"));
    statusbar->addPermanentWidget(current_label);

    _current_page_edit = new QLineEdit(statusbar);
    statusbar->addPermanentWidget(_current_page_edit);
    _current_page_edit->setFixedWidth(50);
    connect(_current_page_edit, SIGNAL(returnPressed()), this, SLOT(current_page_edit_returnPressed()));

    auto previous_page = new DToolButton(statusbar);
    previous_page->setText(tr("Previous Page"));
    statusbar->addPermanentWidget(previous_page);
    connect(previous_page, SIGNAL(pressed()), this, SLOT(previous_page_pressed()));
    previous_page->setShortcut(QKeySequence(Qt::Key_Left));

    auto next_screen = new DToolButton(statusbar);
    next_screen->setText(tr("Next Screen"));
    statusbar->addPermanentWidget(next_screen);
    connect(next_screen, SIGNAL(pressed()), this, SLOT(next_screen_pressed()));
    next_screen->setShortcut(QKeySequence(Qt::Key_Space));

    auto next_page = new DToolButton(statusbar);
    next_page->setText(tr("Next Page"));
    statusbar->addPermanentWidget(next_page);
    connect(next_page, SIGNAL(pressed()), this, SLOT(next_page_pressed()));
    next_page->setShortcut(QKeySequence(Qt::Key_Right));

    // 侧栏
    _ldock = new QDockWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea, _ldock);
    _ldock->setObjectName("leftdock");
    _ldock->setMinimumWidth(150);
    _ldock->setMaximumWidth(500);

    _ldock->setWindowTitle(tr("Library"));
    _ldock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
    //_ldock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    _ldock->installEventFilter(this);

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
    _content = new TextContent(this);
    this->setCentralWidget(_content);

    _content->setReadOnly(true);
    _content->setMinimumWidth(300);
    _content->setFocusPolicy(Qt::NoFocus);
    _content->setFontPointSize(12);
}

void MainWindow::init_ui()
{
    this->resize(ReaderConfig::Instance()->windowSize());
    this->setWindowState(static_cast<Qt::WindowState>(ReaderConfig::Instance()->windowState()));

    auto width = ReaderConfig::Instance()->sidebarWidth();
    if(width == 0)
        width = _ldock->minimumWidth();
    this->resizeDocks({_ldock}, {width}, Qt::Horizontal);
}

void MainWindow::init_shortcuts()
{
    // 标题栏按钮
    auto topbar_actions = this->titlebar()->findChildren<QToolButton*>();
    foreach(QToolButton *action, topbar_actions)
    {
        auto shortcut = ReaderConfig::Instance()->shortcutKeys(action->text());
        if(shortcut.isEmpty())
            continue;

        action->setShortcut(shortcut);
    }

    // 工具栏
    auto toolbar_actions = _toolbar->findChildren<QToolButton*>();
    foreach(QToolButton *action, toolbar_actions)
    {
        auto shortcut = ReaderConfig::Instance()->shortcutKeys(action->text());
        if(shortcut.isEmpty())
            continue;

        action->setShortcut(shortcut);
    }

    // 状态栏
    auto statusbar_actions = this->statusBar()->findChildren<QToolButton*>();
    foreach(QToolButton *action, statusbar_actions)
    {
        auto shortcut = ReaderConfig::Instance()->shortcutKeys(action->text());
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
    _autoscroll_timer->setInterval(500);
    connect(_autoscroll_timer, SIGNAL(timeout()), this, SLOT(autoscroll_timer_timeout()));
}

void MainWindow::library_clicked()
{
    _ldock->setVisible(!_ldock->isVisible());
}

void MainWindow::import_file()
{
    auto lst = DFileDialog::getOpenFileNames(this, "选择文件", ".", "Text Files (*.txt)");
    if(lst.empty())
        return;

    for(auto it = lst.begin(); it != lst.end(); it++)
    {
        qDebug() << *it << endl;

        QFileInfo fi(*it);

        auto item = new QListWidgetItem(fi.baseName(), _booklist);
        item->setData(Qt::UserRole, *it);
        item->setSizeHint(QSize(100, 30));
        _booklist->addItem(item);

        ReaderConfig::Instance()->addBook(*it);
    }
}

void MainWindow::import_directory()
{
    auto dirpath = DFileDialog::getExistingDirectory(this, "选择文件", ".");
    if(dirpath.isEmpty())
        return;

    auto dir = QDir(dirpath);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QString("*.txt;").split(";"));
    auto lst = dir.entryInfoList();

    for(auto it = lst.begin(); it != lst.end(); it++)
    {
        qDebug() << *it << endl;

        auto item = new QListWidgetItem(it->baseName(), _booklist);
        item->setData(Qt::UserRole, it->absoluteFilePath());
        item->setSizeHint(QSize(100, 30));
        _booklist->addItem(item);

        ReaderConfig::Instance()->addBook(it->absoluteFilePath());
    }
}

void MainWindow::remove_novel()
{
    auto item = _booklist->currentItem();
    if(item == nullptr)
        return;

    if(QMessageBox::question(this, "删除确认", "是否从书库中删除此项？") != QMessageBox::Yes)
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

void MainWindow::show_bookmarks()
{
    Bookmarks dlg(this);
    dlg.exec();
}

void MainWindow::view_contents_clicked()
{
    if(_booklist->currentRow() == -1)
        return;

    BookContents dlg(_browser, this);
    auto ret = dlg.exec();
    if(ret != QDialog::Accepted)
        return;

    _content->setText(_browser->currentPageContent());
    _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
}

void MainWindow::add_bookmark_clicked()
{
    ReaderConfig::Instance()->addBookmark(_browser->name(), _browser->currentPage());
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

    _browser = new BookBrowser();

    this->titlebar()->setTitle(_browser->name());

    if(!_browser->isAvailable())
    {
        QMessageBox box(QMessageBox::Information, "文件未找到", QString("以下文件未找到:\n%1").arg(filepath), QMessageBox::Ok, this);
        box.exec();
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
        return;

    _content->setText(_browser->currentPageContent());
    _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
}

void MainWindow::next_page_pressed()
{
    if(_booklist->currentRow() == -1)
        return;

    if(!_browser->moveNext())
        return;

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
        _current_page_edit->setText(QString("%1").arg(_browser->currentPage() + 1));
        return;
    }

    _content->setText(_browser->currentPageContent());
}

void MainWindow::view_shotcuts()
{
    ReaderSettings dlg(this);
    if(dlg.exec() == QDialog::Accepted)
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
}

void MainWindow::select_font_color()
{
    auto color = QColorDialog::getColor(_content->textColor(), this);
    _content->setTextColor(color);
}

void MainWindow::select_background_color()
{
    auto color = QColorDialog::getColor(_content->textColor(), this);
    _content->setTextBackgroundColor(color);
}

void MainWindow::show_dark_mode()
{
    _content->setTextBackgroundColor(QColor(Qt::GlobalColor::black));
    _content->setTextColor(QColor(Qt::GlobalColor::lightGray));
}

void MainWindow::show_full_screen()
{
    _state_before_fullscreen = this->saveState();
    _window_state_before_fullscreen = this->windowState();

    this->showFullScreen();
    _ldock->hide();
    _toolbar->hide();
}

void MainWindow::set_auto_scrolling()
{
    auto action = _toolbar->findChild<QAction*>("set_auto_scrolling");
    if(action == nullptr)
        return;

    if(action->isChecked())
    {
        action->setChecked(false);
        _autoscroll_timer->stop();
    }
    else
    {
        action->setChecked(true);
        _autoscroll_timer->start();
    }
}

void MainWindow::autoscroll_timer_timeout()
{
    _content->moveCursor(QTextCursor::Down);
    if(_content->textCursor().atEnd())
        next_page_pressed();
}

void MainWindow::view_instructions()
{
    QDesktopServices::openUrl(QUrl("https://www.listera.top"));
}

void MainWindow::send_feedback()
{
    QDesktopServices::openUrl(QUrl("https://www.listera.top"));
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        auto keve = static_cast<QKeyEvent*>(event);

        if(keve->key() == Qt::Key_Escape)
        {
            if(this->isFullScreen())
            {
                this->showNormal();
                this->restoreState(_state_before_fullscreen);
                this->setWindowState(_window_state_before_fullscreen);
            }
        }
        else if(keve->key() == Qt::Key_Return || keve->key() == Qt::Key_Enter)
        {
            if(target->objectName() == "linespace")
            {
                auto edit = static_cast<QLineEdit*>(target);

                auto txt = edit->text().trimmed();
                bool ok = false;
                auto linespace = txt.toDouble(&ok);
                if(ok)
                {
                    QTextBlockFormat textBlockFormat;
                    textBlockFormat.setBottomMargin(linespace);
                    _content->textCursor().setBlockFormat(textBlockFormat);
                }
            }
            else if(target->objectName() == "jumptoratio")
            {
                auto edit = static_cast<QLineEdit*>(target);

                auto txt = edit->text().trimmed();
                bool ok = false;
                auto ratio = txt.toFloat(&ok);
                if(ok)
                {
                    _browser->moveToRatio(ratio / 100);

                    _content->setText(_browser->currentPageContent());
                    _current_page_edit->setText(QString("%1").arg(_browser->currentPage()));
                }
            }
        }

        this->setFocus();
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
    ReaderConfig::Instance()->setSidebarWidth(_ldock->width());
}
