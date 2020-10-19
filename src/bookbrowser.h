#ifndef CURRENTBOOK_H
#define CURRENTBOOK_H

#include <QFile>
#include <QMap>
#include <QStringList>
#include <QTextCodec>

class BookBrowser
{
public:
    BookBrowser(const QString &book_name = QString());
    ~BookBrowser();

public:
    // 书名
    QString name() { return _book_name; }

    // 是否可用，主要用于判断分页过程是否正常完成
    bool isAvailable() { return _codec != nullptr; }

    // 总页数
    int pageNumber() { return _page_titles.size(); }

    QStringList pageTitles() { return _page_titles; }
    QString pageTitle(int page);
    int pageIndex(const QString &page_title);

    // 页面内容
    QString pageContent(int page);
    QString pageContent(const QString &page_name);

    QString pagePreview(int page, int num = 800);
    QString pagePreview(const QString &page_name, int num = 800);

    // 当前页面
    // 从 0 开始
    int currentPage() { return _current_page; }
    int nextPage() { return _current_page < _page_titles.size() - 1 ? _current_page + 1 : _current_page; }
    int previousPage() { return _current_page > 0 ? _current_page - 1 : _current_page; }

    QString currentPageTitle() { return pageTitle(_current_page); }
    QString currentPageContent() { return pageContent(_current_page); }

    bool moveNext();
    bool movePrevious();
    bool moveToPage(int page);
    bool moveToRatio(float ratio);

private:
    // 分页。最少 1 页
    void _bookPaging(const QString &file_path);

    qint64 _getPageLength(int page);
    qint64 _getPageLength(const QString &page_title);

private:
    QString _book_name;

    QFile *_file;
    QTextCodec *_codec;
    QList<QString> _page_titles;
    QList<qint64> _page_poses;

    int _current_page;
};

#endif // CURRENTBOOK_H
