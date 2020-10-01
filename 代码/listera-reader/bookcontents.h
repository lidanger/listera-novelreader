#ifndef BOOKCONTENTS_H
#define BOOKCONTENTS_H

#include <DDialog>

DWIDGET_USE_NAMESPACE

class BookContents : public DDialog
{
    Q_OBJECT

public:
    BookContents(QWidget *parent = nullptr);
};

#endif // BOOKCONTENTS_H
