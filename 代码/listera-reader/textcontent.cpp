#include "textcontent.h"

TextContent::TextContent(QWidget *parent)
    : DTextEdit (parent)
{

}

TextContent::TextContent(const QString &text, QWidget *parent)
    : DTextEdit (text, parent)
{

}
