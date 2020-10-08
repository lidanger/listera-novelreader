#ifndef READINGHISTORY_H
#define READINGHISTORY_H

#include <DDialog>

DWIDGET_USE_NAMESPACE

class Bookmarks : public DDialog
{
    Q_OBJECT

public:
    Bookmarks(QWidget *parent = nullptr);
};

#endif // READINGHISTORY_H
