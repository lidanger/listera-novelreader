#ifndef READINGHISTORY_H
#define READINGHISTORY_H

#include <DDialog>

DWIDGET_USE_NAMESPACE

class ReadingHistory : public DDialog
{
    Q_OBJECT

public:
    ReadingHistory(QWidget *parent = nullptr);
};

#endif // READINGHISTORY_H
