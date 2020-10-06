#ifndef BOOKCONTENTS_H
#define BOOKCONTENTS_H

#include <DDialog>
#include <QListWidget>
#include <QTextBrowser>

DWIDGET_USE_NAMESPACE

class BookContents : public DDialog
{
    Q_OBJECT

public:
    BookContents(QString file_path, QTextCodec *codec, QList<QString> toc_names, QMap<QString, qint64> toc_indexs, int current_index = 0, QWidget *parent = nullptr);

public:
    QString selectedTocItem() { return _toc_item; }
    int selectedTocItemIndex() { return _toc_item_index; }

private slots:
    void toc_list_currentRowChanged(int currentRow);
    void toc_list_itemDoubleClicked(QListWidgetItem *item);

private:
    QString _file_path;
    QTextCodec *_codec;

    QListWidget *_toc_list;
    QTextBrowser *_preview_text;

    QString _toc_item;
    int _toc_item_index = -1;
};

#endif // BOOKCONTENTS_H
