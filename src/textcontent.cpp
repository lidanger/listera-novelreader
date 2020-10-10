#include "readerconfig.h"
#include "textcontent.h"
#include "utils.h"

#include <QEvent>
#include <QResizeEvent>
#include <QTextDocumentFragment>

TextContent::TextContent(QWidget *parent)
    :DTextEdit (parent)
{
    init_ui();
}

void TextContent::init_ui()
{
    this->setReadOnly(true);
    this->setMinimumWidth(300);
    this->setFontPointSize(12);
    this->setAcceptRichText(false);
    //this->setFocusPolicy(Qt::ClickFocus);
    this->installEventFilter(this);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(_customContextMenuRequested(const QPoint &)));

    _menu = new QMenu(this);
    auto action = _menu->addAction(tr("Copy"), this, SLOT(copy()));
    action->setShortcut(QKeySequence::Copy);
    action->setObjectName("copy");
    this->addAction(action);
    action = _menu->addAction(tr("Select All"), this, SLOT(selectAll()));
    action->setShortcut(QKeySequence::SelectAll);
    action->setObjectName("selectall");
    this->addAction(action);
    _menu->addSeparator();
    action = _menu->addAction(tr("Web Search"), this, SLOT(_web_search()));
    action->setObjectName("websearch");
    this->addAction(action);
    _menu->addSeparator();
    action = _menu->addAction(tr("Web Translate"), this, SLOT(_translate()));
    action->setObjectName("translate");
    this->addAction(action);
}

void TextContent::_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);

    if(this->textCursor().selection().isEmpty())
    {
        _menu->findChild<QAction*>("copy")->setVisible(false);
        _menu->findChild<QAction*>("websearch")->setVisible(false);
        _menu->findChild<QAction*>("translate")->setVisible(false);
    }
    else
    {
        _menu->findChild<QAction*>("copy")->setVisible(true);
        _menu->findChild<QAction*>("websearch")->setVisible(true);
        _menu->findChild<QAction*>("translate")->setVisible(true);
    }

    _menu->popup(QCursor::pos());
}

void TextContent::_web_search()
{
    auto txt = this->textCursor().selectedText();
    if(txt.isEmpty())
        return;

    auto engine = ReaderConfig::Instance()->searchEngine();
    startWebSearch(txt, engine);
}

void TextContent::_translate()
{
    //auto lo = QLocale();
    auto txt = this->textCursor().selectedText();
    if(txt.isEmpty())
        return;

    auto engine = ReaderConfig::Instance()->translateEngine();
    startWebTranslate(txt, engine);
}

bool TextContent::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::FocusIn)
    {
        if(target == this)
        {
            if(this->parent() != nullptr)
            {
                auto wt = static_cast<QWidget*>(this->parent());
                wt->setFocus();
            }

            event->ignore();
            return true;
        }
    }

    return DTextEdit::eventFilter(target, event);
}
