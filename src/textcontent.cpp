#include "readerconfig.h"
#include "textcontent.h"
#include "utils.h"

#include <QEvent>
#include <QResizeEvent>
#include <QTextDocumentFragment>

TextContent::TextContent(QWidget *parent)
    :QTextEdit (parent)
{
    _background_color.setRed(204);
    _background_color.setGreen(250);
    _background_color.setBlue(226);

    init_ui();
}

void TextContent::init_ui()
{
    this->setReadOnly(true);
    this->setMinimumWidth(300);
    this->setFontPointSize(12);
    this->setFrameShape(QFrame::NoFrame);
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

    return QTextEdit::eventFilter(target, event);
}

void TextContent::refresh()
{
    QString body;
    int last = 0;
    for(int i = 0; i < _origin_text.length(); i++)
    {
        // 换行或结尾为一行
        if(_origin_text[i] != '\n' && i != _origin_text.length() - 1)
            continue;

        auto line = _origin_text.mid(last, i - last + 1);
        last = i + 1;

        // 空行转义
        if(line.trimmed().isEmpty())
        {
            if(_remove_empty_line)
                continue;

            line = QChar::Space;
        }

        // 回车换行符过滤
        for(int j = line.length() - 1; j > 0; j--)
        {
            if(line[j] == '\r' || line[j] == '\n')
                continue;

            line = line.left(j + 1);
            break;
        }

        body.append(QString("<p>%1</p>").arg(line));
    }

    QString style;
    style.append("white-space:pre-wrap;");
    style.append(QString("font-family:'%1';").arg(_font.family()));
    style.append(QString("font-size:%1pt;").arg(_font.pointSizeF()));
    style.append(QString("font-weight:%1;").arg(_font.weight()));
    style.append(QString("line-height:%1;").arg(_line_space));
    style.append(QString("letter-spacing:%1pt;").arg(_char_space));
    if(_darkmode)
    {
        style.append(QString("color:rgb(200,200,200);"));
        style.append(QString("background-color:rgb(50,60,50);"));
    }
    else
    {
        style.append(QString("color:rgb(%1,%2,%3);")
                     .arg(_text_color.red())
                     .arg(_text_color.green())
                     .arg(_text_color.blue()));
        style.append(QString("background-color:rgb(%1,%2,%3);")
                     .arg(_background_color.red())
                     .arg(_background_color.green())
                     .arg(_background_color.blue()));
    }
    if(!_background_image_file.isEmpty())
        style.append(QString("background-image:url(%1);")
                     .arg(_background_image_file));

    QString pstyle;
    pstyle.append(QString("margin:%1px 6px %1px 6px;").arg(_para_space));

    auto html = QString("<html><head><style>body{%1}p{%2}</style></head><body>%3</body></html>").arg(style).arg(pstyle).arg(body);
    this->QTextEdit::setHtml(html);
}

void TextContent::setBackgroundColor(const QColor &c)
{
    if(!c.isValid())
        return;

    _background_color = c;
    refresh();
}

void TextContent::setBackgroundImage(const QString &file_path)
{
    _background_image_file = file_path;
    refresh();
}

void TextContent::setLineSpacing(double spacing)
{
    if(spacing < 1)
        return;

    _line_space = spacing;
    refresh();
}

void TextContent::setCharSpacing(double spacing)
{
    if(spacing < 0)
        return;

    _char_space = spacing;
    refresh();
}

void TextContent::setParagraphSpaceing(int spacing)
{
    if(spacing < 0)
        return;

    _para_space = spacing;
    refresh();
}

void TextContent::setRemoveEmptyLine(bool remove)
{
    _remove_empty_line = remove;
    refresh();
}

void TextContent::setDarkMode(bool dark)
{
    _darkmode = dark;
    refresh();
}

void TextContent::setFont(const QFont &f)
{
    _font = f;
    refresh();
}

void TextContent::setFontFamily(const QString &fontFamily)
{
    if(fontFamily.isEmpty())
        return;

    _font.setFamily(fontFamily);
    refresh();
}

void TextContent::setFontItalic(bool italic)
{
    _font.setItalic(italic);
    refresh();
}

void TextContent::setFontPointSize(qreal s)
{
    if(s <= 0)
        return;

    _font.setPointSizeF(s);
    refresh();
}

void TextContent::setFontUnderline(bool underline)
{
    _font.setUnderline(underline);
    refresh();
}

void TextContent::setFontWeight(int w)
{
    _font.setWeight(w);
    refresh();
}

void TextContent::setPlainText(const QString &text)
{
    _origin_text = text;
    refresh();
}

void TextContent::setText(const QString &text)
{
    _origin_text = text;
    refresh();
}

void TextContent::setTextColor(const QColor &c)
{
    if(!c.isValid())
        return;

    _text_color = c;
    refresh();
}
