#ifndef TEXTCONTENT_H
#define TEXTCONTENT_H

#include <DTextEdit>

DWIDGET_USE_NAMESPACE

class TextContent : public DTextEdit
{
public:
    TextContent(QWidget *parent = nullptr);
};

#endif // TEXTCONTENT_H
