#include "advancedoptions.h"
#include "bookcontents.h"
#include "jumpratio.h"
#include "mainwindow.h"
#include "readerconfig.h"
#include "readinghistory.h"
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
#include "utils.h"
#include <QTextBlock>

MainWindow::MainWindow()
    : DMainWindow()
{
    init_ui();
    init_shortcuts();
    init_data();
}

void MainWindow::on_sidebar_clicked()
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

void MainWindow::show_history()
{
    ReadingHistory dlg(this);
    dlg.exec();
}

void MainWindow::view_book_contents()
{
    if(_booklist->currentRow() == -1)
        return;

    auto filepath = _booklist->currentItem()->data(Qt::UserRole).toString();

    BookContents dlg(filepath, _codec, _toc_names, _toc_indexs, _current_page, this);
    dlg.exec();

    auto toc_item_index = dlg.selectedTocItemIndex();
    if(toc_item_index < 0)
        return;

    _current_page = toc_item_index;
    _current_page_edit->setText(QString("%1").arg(_current_page + 1));

    ReaderConfig::Instance()->setBookCurrentPage(_current_book, _current_page);

    QFile file(filepath);
    if(!file.exists())
    {
        QMessageBox box(QMessageBox::Information, "文件未找到", QString("以下文件未找到:\n%1").arg(filepath), QMessageBox::Ok, this);
        box.exec();
        return;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto pagebegin = _toc_indexs[_toc_names[_current_page]];
    auto pageend = 9223372036854775807;
    if(_current_page < _toc_indexs.size() - 1)
        pageend = _toc_indexs[_toc_names[_current_page + 1]];

    file.seek(pagebegin);
    auto content = file.read(pageend - pagebegin);
    auto txt = _codec->toUnicode(content);
    _content->setText(txt);

    file.close();
}

void MainWindow::show_advanced_options()
{
    AdvancedOptions dlg(this);
    dlg.exec();
}

void MainWindow::show_jump_ratio()
{
    JumpRatio dlg(this);
    dlg.exec();
}

void MainWindow::booklist_currentRowChanged(int currentRow)
{
    // 获取文件
    auto item = _booklist->item(currentRow);
    _current_book = item->text();
    auto filepath = item->data(Qt::UserRole).toString();

    QFile file(filepath);
    if(!file.exists())
    {
        QMessageBox box(QMessageBox::Information, "文件未找到", QString("以下文件未找到:\n%1").arg(filepath), QMessageBox::Ok, this);
        box.exec();
        return;
    }

    _codec = GetCorrectTextCodec(filepath);
    if(_codec == nullptr)
        return;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    ReaderConfig::Instance()->setCurrentBook(_current_book);
    this->titlebar()->setTitle(_current_book);

    // 分页
    _toc_indexs.clear();
    _toc_names.clear();

    do
    {
        auto pos = file.pos();
        auto line = file.readLine();
        auto txt = _codec->toUnicode(line).trimmed();

        if(is_toc_item(txt))
        {
            _toc_indexs[txt] = pos;
            _toc_names.push_back(txt);
        }

    } while(!file.atEnd());

    // 加载页
    if(_toc_indexs.size() < 2)
    {
        file.seek(0);
        auto content = file.readAll();
        auto txt = _codec->toUnicode(content);
        _content->setText(txt);

        _current_page = 0;

        _total_pages_edit->setText(QString("%1").arg(1));
    }
    else
    {
        _current_page = ReaderConfig::Instance()->bookCurrentPage(_current_book);

        auto pagebegin = _toc_indexs[_toc_names[_current_page]];
        auto pageend = 9223372036854775807;
        if(_current_page < _toc_indexs.size() - 1)
            pageend = _toc_indexs[_toc_names[_current_page + 1]];

        file.close();
        if(!file.open(QIODevice::ReadOnly))
            return;

        file.seek(pagebegin);
        auto content = file.read(pageend - pagebegin);
        auto txt = _codec->toUnicode(content);
        _content->setText(txt);

        _total_pages_edit->setText(QString("%1").arg(_toc_indexs.size()));
    }

    _current_page_edit->setText(QString("%1").arg(_current_page + 1));

    file.close();
}

void MainWindow::previous_page_pressed()
{
    if(_booklist->currentRow() == -1)
        return;

    auto filepath = _booklist->currentItem()->data(Qt::UserRole).toString();

    if(_current_page == 0)
        return;    

    --_current_page;
    _current_page_edit->setText(QString("%1").arg(_current_page + 1));

    ReaderConfig::Instance()->setBookCurrentPage(_current_book, _current_page);

    QFile file(filepath);
    if(!file.exists())
    {
        QMessageBox box(QMessageBox::Information, "文件未找到", QString("以下文件未找到:\n%1").arg(filepath), QMessageBox::Ok, this);
        box.exec();
        return;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto pagebegin = _toc_indexs[_toc_names[_current_page]];
    auto pageend = _toc_indexs[_toc_names[_current_page + 1]];

    file.seek(pagebegin);
    auto content = file.read(pageend - pagebegin);
    auto txt = _codec->toUnicode(content);
    _content->setText(txt);

    file.close();
}

void MainWindow::next_page_pressed()
{
    if(_booklist->currentRow() == -1)
        return;

    auto filepath = _booklist->currentItem()->data(Qt::UserRole).toString();

    if(_current_page == _toc_indexs.size() - 1)
        return;

    ++_current_page;
    _current_page_edit->setText(QString("%1").arg(_current_page + 1));

    ReaderConfig::Instance()->setBookCurrentPage(_current_book, _current_page);

    QFile file(filepath);
    if(!file.exists())
    {
        QMessageBox box(QMessageBox::Information, "文件未找到", QString("以下文件未找到:\n%1").arg(filepath), QMessageBox::Ok, this);
        box.exec();
        return;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto pagebegin = _toc_indexs[_toc_names[_current_page]];
    auto pageend = 9223372036854775807;
    if(_current_page < _toc_indexs.size() - 1)
        pageend = _toc_indexs[_toc_names[_current_page + 1]];

    file.seek(pagebegin);
    auto content = file.read(pageend - pagebegin);
    auto txt = _codec->toUnicode(content);
    _content->setText(txt);

    file.close();
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

    auto txt = _current_page_edit->text().trimmed();
    bool ok = false;
    auto page_index = txt.toInt(&ok);
    if(!ok)
        return;

    if(page_index < 1 || page_index > _toc_indexs.size() || page_index - 1 == _current_page)
        return;

    if(_booklist->currentRow() == -1)
        return;

    auto filepath = _booklist->currentItem()->data(Qt::UserRole).toString();

    _current_page = page_index - 1;
    ReaderConfig::Instance()->setBookCurrentPage(_current_book, _current_page);

    QFile file(filepath);
    if(!file.exists())
    {
        QMessageBox box(QMessageBox::Information, "文件未找到", QString("以下文件未找到:\n%1").arg(filepath), QMessageBox::Ok, this);
        box.exec();
        return;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto pagebegin = _toc_indexs[_toc_names[_current_page]];
    auto pageend = 9223372036854775807;
    if(_current_page < _toc_indexs.size() - 1)
        pageend = _toc_indexs[_toc_names[_current_page + 1]];

    file.seek(pagebegin);
    auto content = file.read(pageend - pagebegin);
    txt = _codec->toUnicode(content);
    _content->setText(txt);

    file.close();
}

void MainWindow::init_ui()
{
    this->setMinimumSize(1000, 600);

    this->resize(ReaderConfig::Instance()->windowSize());
    this->setWindowState(static_cast<Qt::WindowState>(ReaderConfig::Instance()->windowState()));

    // 获取应用程序对象指针
    auto app = static_cast<DApplication*>(QApplication::instance());

    // 关于
    auto about = new DAboutDialog(this);

    about->setVersion(app->applicationVersion());
    about->setCompanyLogo(QPixmap(":/images/logo_3.png").scaledToWidth(150));
    about->setDescription(app->applicationDescription());
    about->setProductIcon(app->productIcon());
    about->setProductName(app->productName());
    about->setWebsiteLink(app->applicationHomePage());
    about->setWebsiteName("www.listera.top");

    app->setAboutDialog(about);

    // 标题栏
    auto titlebar = this->titlebar();
    titlebar->setIcon(app->productIcon());
    titlebar->setTitle(app->productName());

    // 目录
    auto toc = new DToolButton(titlebar);
    toc->setText("目录");
    titlebar->addWidget(toc, Qt::AlignRight);
    connect(toc, SIGNAL(clicked()), this, SLOT(view_book_contents()));

    // 添加书签
    auto mark = new DToolButton(titlebar);
    mark->setText("书签");
    titlebar->addWidget(mark, Qt::AlignRight);

    // 黑暗模式
    auto dark = new DToolButton(titlebar);
    dark->setText("关灯");
    titlebar->addWidget(dark, Qt::AlignRight);

    // 侧栏
    auto sidebar = new DToolButton(titlebar);
    sidebar->setText("书库");
    titlebar->addWidget(sidebar, Qt::AlignRight);
    connect(sidebar, SIGNAL(clicked()), this, SLOT(on_sidebar_clicked()));

    // 菜单 文件 书签 选项 视图 帮助
    auto topmenu = new DMenu(titlebar);
    titlebar->setMenu(topmenu);

    auto filemenu = topmenu->addMenu(tr("File"));
    connect(filemenu->addAction(tr("Import File...")), SIGNAL(triggered()), this, SLOT(import_file()));
    connect(filemenu->addAction(tr("Import Directory...")), SIGNAL(triggered()), this, SLOT(import_directory()));
    filemenu->addSeparator();
    connect(filemenu->addAction(tr("Reading History...")), SIGNAL(triggered()), this, SLOT(show_history()));

    auto bmenu = topmenu->addMenu(tr("Bookmark"));
    bmenu->addAction(tr("Add Bookmark"));
    bmenu->addAction(tr("Open Bookmark"));
    bmenu->addAction(tr("Remove Bookmark"));
    bmenu->addAction(tr("Clear Bookmark"));
    bmenu->addSeparator();
    connect(bmenu->addAction(tr("View Book Contents")), SIGNAL(triggered()), this, SLOT(view_book_contents()));

    auto opmenu = topmenu->addMenu(tr("Options"));
    opmenu->addAction(tr("Font"));
    opmenu->addAction(tr("Line Space"));
    opmenu->addAction(tr("Background Color"));
    opmenu->addAction(tr("Font Size"));
    opmenu->addAction(tr("Font Color"));
    opmenu->addAction(tr("Paging Mode"));
    opmenu->addSeparator();
    opmenu->addAction(tr("Dark Mode"));
    opmenu->addSeparator();
    opmenu->addAction(tr("Shotcuts"));
    opmenu->addSeparator();
    connect(opmenu->addAction(tr("Advanced Options")), SIGNAL(triggered()), this, SLOT(show_advanced_options()));

    auto viewmenu = topmenu->addMenu(tr("View"));
    viewmenu->addAction(tr("Toolbar"));
    viewmenu->addAction(tr("Statusbar"));
    viewmenu->addAction(tr("Sidebar"));
    viewmenu->addSeparator();
    connect(viewmenu->addAction(tr("Full Screen")), SIGNAL(triggered()), this, SLOT(showFullScreen()));
    viewmenu->addSeparator();
    viewmenu->addAction(tr("Auto Scrolling"));
    connect(viewmenu->addAction(tr("Jump to Ratio")), SIGNAL(triggered()), this, SLOT(show_jump_ratio()));

    auto helpmenu = topmenu->addMenu(tr("Help"));
    helpmenu->addAction(tr("Instructions"));
    helpmenu->addAction(tr("Feedback"));

    topmenu->addSeparator();

    // 工具栏，可选显示，所有功能都可以通过快捷键来
    auto toolbar = new DToolBar(this);
    this->addToolBar(toolbar);
    toolbar->hide();

    auto test = new DToolButton(toolbar);
    test->setText("测试");
    toolbar->addWidget(test);
    toolbar->setWindowTitle("工具栏");

    // 状态栏 总章 当前章 上一页 下一页 翻屏
    auto statusbar = new DStatusBar(this);
    this->setStatusBar(statusbar);

    statusbar->setFixedHeight(30);
    statusBar()->setStyleSheet(QString( "QStatusBar::item{border: 0px}"));

    auto total_label = new QLabel(statusbar);
    total_label->setText("总页数");
    statusbar->addPermanentWidget(total_label);

    _total_pages_edit = new QLineEdit(statusbar);
    statusbar->addPermanentWidget(_total_pages_edit);
    _total_pages_edit->setFixedWidth(50);
    _total_pages_edit->setReadOnly(true);
    _total_pages_edit->setFocusPolicy(Qt::NoFocus);

    auto current_label = new QLabel(statusbar);
    current_label->setText("当前页");
    statusbar->addPermanentWidget(current_label);

    _current_page_edit = new QLineEdit(statusbar);
    statusbar->addPermanentWidget(_current_page_edit);
    _current_page_edit->setFixedWidth(50);
    connect(_current_page_edit, SIGNAL(returnPressed()), this, SLOT(current_page_edit_returnPressed()));

    auto previous_page = new DToolButton(statusbar);
    previous_page->setText("上一页");
    statusbar->addPermanentWidget(previous_page);
    connect(previous_page, SIGNAL(pressed()), this, SLOT(previous_page_pressed()));
    previous_page->setShortcut(QKeySequence(hotkey_previous_page));

    auto next_screen = new DToolButton(statusbar);
    next_screen->setText("翻屏");
    statusbar->addPermanentWidget(next_screen);
    connect(next_screen, SIGNAL(pressed()), this, SLOT(next_screen_pressed()));
    next_screen->setShortcut(QKeySequence(hotkey_next_screen2));

    auto next_page = new DToolButton(statusbar);
    next_page->setText("下一页");
    statusbar->addPermanentWidget(next_page);
    connect(next_page, SIGNAL(pressed()), this, SLOT(next_page_pressed()));
    next_page->setShortcut(QKeySequence(hotkey_next_page));

    // 内容区
    _content = new TextContent(this);
    this->setCentralWidget(_content);

    _content->setReadOnly(true);
    _content->setMinimumWidth(300);
    _content->setFocusPolicy(Qt::NoFocus);
    _content->setFontPointSize(12);

    // 侧栏
    _ldock = new QDockWidget(this);
    _ldock->setMinimumWidth(150);
    _ldock->setMaximumWidth(500);
    this->addDockWidget(Qt::LeftDockWidgetArea, _ldock);
    auto width = ReaderConfig::Instance()->sidebarWidth();
    if(width == 0)
        width = _ldock->minimumWidth();
    this->resizeDocks({_ldock}, {width}, Qt::Horizontal);

    _ldock->setWindowTitle("书库");
    _ldock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
    //_ldock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    _booklist = new DListWidget(_ldock);
    _ldock->setWidget(_booklist);
    connect(_booklist, SIGNAL(currentRowChanged(int)), this, SLOT(booklist_currentRowChanged(int)));

    _ldock->installEventFilter(this);
    this->installEventFilter(this);
}

void MainWindow::init_shortcuts()
{
    // 菜单
    auto menu_actions = this->titlebar()->menu()->findChildren<QAction*>();
    foreach(QAction *action, menu_actions)
    {
        if(action->isSeparator())
            continue;

        if(!_hotkeys.contains(action->text()))
            continue;

        action->setShortcut(_hotkeys[action->text()]);
    }

    // 工具栏
    auto toolbar_actions = this->titlebar()->menu()->findChildren<QToolButton*>();
    foreach(QToolButton *action, toolbar_actions)
    {
        if(!_hotkeys.contains(action->text()))
            continue;

        action->setShortcut(_hotkeys[action->text()]);
    }

    // 状态栏
    auto statusbar_actions = this->statusBar()->findChildren<QToolButton*>();
    foreach(QToolButton *action, statusbar_actions)
    {
        if(!_hotkeys.contains(action->text()))
            continue;

        action->setShortcut(_hotkeys[action->text()]);
    }
}

void MainWindow::displayShortcuts()
{
//    QRect rect = window()->geometry();
//    QPoint pos(rect.x() + rect.width() / 2,
//               rect.y() + rect.height() / 2);

//    //窗体快捷键
//    QStringList windowKeymaps;
//    windowKeymaps << "addblanktab" << "newwindow" << "savefile"
//                  << "saveasfile" << "selectnexttab" << "selectprevtab"
//                  << "closetab" << "closeothertabs" << "restoretab"
//                  << "openfile" << "incrementfontsize" << "decrementfontsize"
//                  << "resetfontsize" << "togglefullscreen" << "find" << "replace"
//                  << "jumptoline" << "saveposition" << "restoreposition"
//                  << "escape" << "print";

//    QJsonObject shortcutObj;
//    QJsonArray jsonGroups;

//    QJsonObject windowJsonGroup;
//    windowJsonGroup.insert("groupName", QObject::tr("Window"));
//    QJsonArray windowJsonItems;

//    for (const QString &keymap : windowKeymaps) {
//        auto option = m_settings->settings->group("shortcuts.window")->option(QString("shortcuts.window.%1").arg(keymap));
//        QJsonObject jsonItem;
//        jsonItem.insert("name", QObject::tr(option->name().toUtf8().data()));
//        if (keymap != "incrementfontsize" && keymap != "decrementfontsize") {
//            jsonItem.insert("value", option->value().toString().replace("Meta", "Super"));
//        } else if (keymap == "incrementfontsize") {
//            QString strIncrementfontValue = QString(tr("Ctrl+'='"));
//            jsonItem.insert("value", strIncrementfontValue.replace("Meta", "Super"));
//        } else if (keymap == "decrementfontsize" && option->value().toString() == "Ctrl+-") {
//            QString strDecrementfontValue = QString(tr("Ctrl+'-'"));
//            jsonItem.insert("value", strDecrementfontValue.replace("Meta", "Super"));
//        }

//        windowJsonItems.append(jsonItem);
//    }

//    windowJsonGroup.insert("groupItems", windowJsonItems);
//    jsonGroups.append(windowJsonGroup);

//    //编辑快捷键
//    QStringList editorKeymaps;
//    editorKeymaps << "indentline" << "backindentline" << "forwardchar"
//                  << "backwardchar" << "forwardword" << "backwardword"
//                  << "nextline" << "prevline" << "newline" << "opennewlineabove"
//                  << "opennewlinebelow" << "duplicateline" << "killline"
//                  << "killcurrentline" << "swaplineup" << "swaplinedown"
//                  << "scrolllineup" << "scrolllinedown" << "scrollup"
//                  << "scrolldown" << "movetoendofline" << "movetostartofline"
//                  << "movetoend" << "movetostart" << "movetolineindentation"
//                  << "upcaseword" << "downcaseword" << "capitalizeword"
//                  << "killbackwardword" << "killforwardword" << "forwardpair"
//                  << "backwardpair" << "selectall" << "copy" << "cut"
//                  << "paste" << "transposechar" << "setmark" << "exchangemark"
//                  << "copylines" << "cutlines" << "joinlines" << "togglereadonlymode"
//                  << "togglecomment" << "removecomment" << "undo" << "redo" << "switchbookmark" << "movetoprebookmark"
//                  << "movetonextbookmark" << "mark";

//    QJsonObject editorJsonGroup;
//    editorJsonGroup.insert("groupName", tr("Editor"));
//    QJsonArray editorJsonItems;

//    for (const QString &keymap : editorKeymaps) {
//        auto option = m_settings->settings->group("shortcuts.editor")->option(QString("shortcuts.editor.%1").arg(keymap));
//        QJsonObject jsonItem;
//        jsonItem.insert("name", QObject::tr(option->name().toUtf8().data()));
//        jsonItem.insert("value", option->value().toString().replace("Meta", "Super"));
//        editorJsonItems.append(jsonItem);
//    }
//    editorJsonGroup.insert("groupItems", editorJsonItems);
//    jsonGroups.append(editorJsonGroup);

//    //设置快捷键
//    QStringList setupKeymaps;
//    setupKeymaps << "help" << "displayshortcuts";

//    QJsonObject setupJsonGroup;
//    setupJsonGroup.insert("groupName", tr("Settings"));
//    QJsonArray setupJsonItems;

//    for (const QString &keymap : setupKeymaps) {
//        auto option = m_settings->settings->group("shortcuts.window")->option(QString("shortcuts.window.%1").arg(keymap));
//        QJsonObject jsonItem;
//        jsonItem.insert("name", QObject::tr(option->name().toUtf8().data()));
//        jsonItem.insert("value", option->value().toString().replace("Meta", "Super"));
//        setupJsonItems.append(jsonItem);
//    }
//    setupJsonGroup.insert("groupItems", setupJsonItems);
//    jsonGroups.append(setupJsonGroup);

//    shortcutObj.insert("shortcut", jsonGroups);

//    QJsonDocument doc(shortcutObj);

//    QStringList shortcutString;
//    QString param1 = "-j=" + QString(doc.toJson().data());
//    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());
//    shortcutString << param1 << param2;

//    QProcess *shortcutViewProcess = new QProcess();
//    shortcutViewProcess->startDetached("deepin-shortcut-viewer", shortcutString);

//    connect(shortcutViewProcess, SIGNAL(finished(int)), shortcutViewProcess, SLOT(deleteLater()));
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::Resize)
    {
        if(target == this)
        {
            ReaderConfig::Instance()->setWindowSize(this->size());
            ReaderConfig::Instance()->setWindowState(static_cast<int>(this->windowState()));
        }
        else if(target == _ldock)
        {
            ReaderConfig::Instance()->setSidebarWidth(_ldock->width());
        }
    }
    else if(event->type() == QEvent::KeyPress)
    {
        auto keve = static_cast<QKeyEvent*>(event);
        if(keve->key() == Qt::Key_Escape)
        {
            if(this->isFullScreen())
                this->showNormal();
        }
        this->setFocus();
    }

    // 处理其他消息
    return QWidget::eventFilter(target, event);
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
