#include "mainwindow.h"
#include "readerconfig.h"
#include <DTitlebar>
#include <QApplication>
#include <DApplication>
#include <DAboutDialog>
#include <DToolButton>
#include <DToolBar>
#include <QDockWidget>
#include <DStatusBar>
#include <QSignalMapper>
#include <DListWidget>
#include <DTextEdit>
#include <QtDebug>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QResizeEvent>

MainWindow::MainWindow()
    : DMainWindow()
{
    init_ui();
}

void MainWindow::on_sidebar_clicked()
{
    _ldock->setVisible(!_ldock->isVisible());
}

void MainWindow::init_ui()
{
    this->setMinimumSize(1000, 600);

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
    filemenu->addAction(tr("Import File..."));
    filemenu->addAction(tr("Import Directory..."));
    filemenu->addAction(tr("Reading History..."));

    auto bmenu = topmenu->addMenu(tr("Bookmark"));
    bmenu->addAction(tr("Add Bookmark"));
    bmenu->addAction(tr("Open Bookmark"));
    bmenu->addAction(tr("Remove Bookmark"));
    bmenu->addAction(tr("Clear Bookmark"));
    bmenu->addSeparator();
    bmenu->addAction(tr("View TOC"));

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
    opmenu->addAction(tr("Advanced Options"));

    auto viewmenu = topmenu->addMenu(tr("View"));
    viewmenu->addAction(tr("Toolbar"));
    viewmenu->addAction(tr("Statusbar"));
    viewmenu->addAction(tr("Sidebar"));
    viewmenu->addSeparator();
    viewmenu->addAction(tr("Full Screen"));
    viewmenu->addSeparator();
    viewmenu->addAction(tr("Auto Scrolling"));
    viewmenu->addAction(tr("Jump to Ratio"));

    auto helpmenu = topmenu->addMenu(tr("Help"));
    helpmenu->addAction(tr("Instructions"));
    helpmenu->addAction(tr("Feedback"));

    topmenu->addSeparator();

    // 工具栏，可选显示，所有功能都可以通过快捷键来
    auto toolbar = new DToolBar(this);
    this->addToolBar(toolbar);

    auto test = new DToolButton(toolbar);
    test->setText("测试");
    toolbar->addWidget(test);
    toolbar->setWindowTitle("工具栏");

    // 状态栏 总章 当前章 上一页 下一页 翻屏
    auto statusbar = new DStatusBar(this);
    this->setStatusBar(statusbar);

    statusbar->setFixedHeight(30);
    statusBar()->setStyleSheet(QString( "QStatusBar::item{border: 0px}"));

    statusbar->showMessage("dfksldfklsd");

    auto total_label = new QLabel(statusbar);
    total_label->setText("总章数");
    statusbar->addPermanentWidget(total_label);

    auto total_edit = new QLineEdit(statusbar);
    statusbar->addPermanentWidget(total_edit);
    total_edit->setFixedWidth(50);

    auto current_label = new QLabel(statusbar);
    current_label->setText("当前章数");
    statusbar->addPermanentWidget(current_label);

    auto current_edit = new QLineEdit(statusbar);
    statusbar->addPermanentWidget(current_edit);
    current_edit->setFixedWidth(50);

    auto previous_page = new DToolButton(statusbar);
    previous_page->setText("上一页");
    statusbar->addPermanentWidget(previous_page);

    auto next_screen = new DToolButton(statusbar);
    next_screen->setText("翻屏");
    statusbar->addPermanentWidget(next_screen);

    auto next_page = new DToolButton(statusbar);
    next_page->setText("下一页");
    statusbar->addPermanentWidget(next_page);

    // 内容区
    _content = new TextContent(this);
    this->setCentralWidget(_content);

    _content->setReadOnly(true);
    _content->setMinimumWidth(300);

    // 侧栏
    _ldock = new QDockWidget(this);
    _ldock->setMinimumWidth(150);
    _ldock->setMaximumWidth(500);
    this->addDockWidget(Qt::LeftDockWidgetArea, _ldock);
    this->resizeDocks({_ldock}, {_ldock->minimumWidth()}, Qt::Horizontal);

    _ldock->setWindowTitle("书库");
    _ldock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);

    _booklist = new DListWidget(_ldock);
    _ldock->setWidget(_booklist);

    _booklist->addItem("test");
    _booklist->addItem("test2");

    _booklist->item(0)->setSizeHint(QSize(100, 30));

    //_ldock->installEventFilter(this);
    this->installEventFilter(this);
}

void MainWindow::init_shortcuts()
{
    QList<QKeySequence> keyList;
    keyList.append(QKeySequence::Find);
    keyList.append(QKeySequence::Open);
    keyList.append(QKeySequence::Print);
    keyList.append(QKeySequence::Save);
    keyList.append(QKeySequence::Copy);
    keyList.append(QKeySequence(Qt::Key_Left));
    keyList.append(QKeySequence(Qt::Key_Right));
    keyList.append(QKeySequence(Qt::Key_Up));
    keyList.append(QKeySequence(Qt::Key_Down));
    keyList.append(QKeySequence(Qt::Key_PageUp));
    keyList.append(QKeySequence(Qt::Key_PageDown));
    keyList.append(QKeySequence(Qt::Key_Space));
    keyList.append(QKeySequence(Qt::Key_Escape));
    keyList.append(QKeySequence(Qt::Key_F5));
    keyList.append(QKeySequence(Qt::Key_F11));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_1));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_2));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_A));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_H));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_Z));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_1));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_2));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_3));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_D));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_M));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_R));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_Equal));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    keyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    keyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    keyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));

    auto pSigManager = new QSignalMapper(this);

    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(onShortCut(const QString &)));

    foreach (auto key, keyList)
    {
        auto action = new QAction(this);

        action->setShortcut(key);

        this->addAction(action);

        connect(action, SIGNAL(triggered()), pSigManager, SLOT(map()));

        pSigManager->setMapping(action, key.toString());
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::Resize)
    {
        if(target == this)
        {
            ReaderConfig::Instance()->setWindowSize(this->size());
        }
    }

    // 处理其他消息
    return QWidget::eventFilter(target, event);
}
