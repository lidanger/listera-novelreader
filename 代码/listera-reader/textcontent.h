#ifndef TEXTCONTENT_H
#define TEXTCONTENT_H

#include <DTextEdit>

DWIDGET_USE_NAMESPACE

class TextContent : public DTextEdit
{
    Q_OBJECT

public:
    TextContent(QWidget *parent = nullptr);

    TextContent(const QString& text, QWidget* parent = nullptr);
};

#endif // TEXTCONTENT_H
