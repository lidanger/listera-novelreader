#ifndef TEXTCONTENT_H
#define TEXTCONTENT_H

#include <DTextEdit>
#include <QMenu>

DWIDGET_USE_NAMESPACE

class TextContent : public DTextEdit
{
    Q_OBJECT

public:
    TextContent(QWidget *parent = nullptr);

private:
    void init_ui();

private slots:
    void _customContextMenuRequested(const QPoint &pos);
    void _web_search();
    void _translate();

private:
    bool eventFilter(QObject *target, QEvent *event);

private:
    QMenu *_menu;
};

#endif // TEXTCONTENT_H
