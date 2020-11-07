#include "bookbrowser.h"
#include "readerconfig.h"
#include "utils.h"

BookBrowser::BookBrowser(const QString &book_name)
    :_book_name(book_name)
{
    // 从设置中加载必要的信息
    if(_book_name.isEmpty())
        _book_name = ReaderConfig::Instance()->currentBook();

    auto file_path = ReaderConfig::Instance()->bookPath(_book_name);

    _current_page = ReaderConfig::Instance()->bookCurrentPage(_book_name);

    // 检查文件
    if(file_path.isEmpty())
        return;

    _file = new QFile(file_path);
    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return;
    }

    _codec = GetCorrectTextCodec(_file->fileName());

    // 获取分页信息
    _bookPaging();

    // 以只读模式打开
    _file->open(QIODevice::ReadOnly);
}

BookBrowser::~BookBrowser()
{
    if(_file != nullptr)
    {
        _file->close();
        delete _file;
    }
}

QString BookBrowser::pageTitle(int page)
{
     if(page < 0 || page >= _page_titles.count())
         return QString();

     return _page_titles[page];
}

int BookBrowser::pageIndex(const QString &page_title)
{
    if(page_title.isEmpty())
        return -1;

    return _page_titles.indexOf(page_title);
}

QString BookBrowser::pageContent(int page)
{
    if(page < 0 || page > _page_titles.size() - 1)
        return QString();

    qint64 pagebegin = 0;
    auto len = _getPagePos(page, &pagebegin);

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return QString();
    }

    _file->seek(pagebegin);
    auto content = _file->read(len);

    auto txt = _codec->toUnicode(content);

    // 处理首页为空的情况
    if(txt.trimmed().isEmpty())
    {
        txt = pageContent(page + 1);
    }

    return txt;
}

QString BookBrowser::pageContent(const QString &page_name)
{
    if(page_name.isEmpty())
        return QString();

    auto index = _page_titles.indexOf(page_name);
    if(index == -1)
        return QString();

    qint64 pagebegin = 0;
    auto len = _getPagePos(page_name, &pagebegin);

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return QString();
    }

    _file->seek(pagebegin);
    auto content = _file->read(len);

    auto txt = _codec->toUnicode(content);

    // 处理首页为空的情况
    if(txt.trimmed().isEmpty())
    {
        auto index = _page_titles.indexOf(page_name);
        txt = pageContent(index + 1);
    }

    return txt;
}

QString BookBrowser::pagePreview(int page, int num)
{
    if(page < 0 || page > _page_titles.size() - 1)
        return QString();

    auto pagebegin = _page_poses[page];

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return QString();
    }

    _file->seek(pagebegin);
    auto content = _file->read(num);

    return _codec->toUnicode(content);
}

QString BookBrowser::pagePreview(const QString &page_name, int num)
{
    if(page_name.isEmpty())
        return QString();

    auto index = _page_titles.indexOf(page_name);
    if(index != -1)
        return QString();

    auto pagebegin = _page_poses[index];

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return QString();
    }

    _file->seek(pagebegin);
    auto content = _file->read(num);

    return _codec->toUnicode(content);
}

bool BookBrowser::moveNext()
{
    if(_page_titles.isEmpty() || !_file->exists())
        return false;

    if(_current_page == _page_titles.size() - 1)
        return false;

    ++_current_page;
    ReaderConfig::Instance()->setBookCurrentPage(_book_name, _current_page);

    return true;
}

bool BookBrowser::movePrevious()
{
    if(_page_titles.isEmpty() || !_file->exists())
        return false;

    if(_current_page == 0)
        return false;

    --_current_page;
    ReaderConfig::Instance()->setBookCurrentPage(_book_name, _current_page);

    return true;
}

bool BookBrowser::moveToPage(int page)
{
    if(_page_titles.isEmpty() || !_file->exists())
        return false;

    if(page < 0 || page > _page_titles.size() - 1)
        return false;

    _current_page = page;
    ReaderConfig::Instance()->setBookCurrentPage(_book_name, _current_page);

    return true;
}

bool BookBrowser::moveToRatio(float ratio)
{
    if(_page_titles.isEmpty() || !_file->exists())
        return false;

    if(ratio < 0 || ratio > 1)
        return false;

    auto page = (_page_titles.size() - 1) * ratio;
    _current_page = static_cast<int>(page);
    ReaderConfig::Instance()->setBookCurrentPage(_book_name, _current_page);

    return true;
}

void BookBrowser::_bookPaging()
{
    if(_codec == nullptr)
        return;

    if(!_file->open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    auto first_page_name = QObject::tr("First Page");
    _page_titles.push_back(first_page_name);
    _page_poses.push_back(0);

    do
    {
        auto pos = _file->pos();
        auto line = _file->readLine();
        auto txt = _codec->toUnicode(line).trimmed();

        if(txt.startsWith(_book_name))
            txt = txt.right(txt.length() - _book_name.length()).trimmed();

        if(is_toc_item(txt))
        {
            _page_titles.push_back(txt);
            _page_poses.push_back(pos);
        }

    } while(!_file->atEnd());

    _file->close();
}

qint64 BookBrowser::_getPagePos(int page_index, qint64 *page_begin)
{
    *page_begin = _page_poses[page_index];

    auto pageend = 9223372036854775807;
    if(page_index < _page_titles.size() - 1)
        pageend = _page_poses[page_index + 1];

    return pageend - *page_begin;
}

qint64 BookBrowser::_getPagePos(const QString &page_title, qint64 *page_begin)
{
    auto page = _page_titles.indexOf(page_title);

    *page_begin = _page_poses[page];

    auto pageend = 9223372036854775807;
    if(page < _page_titles.size() - 1)
        pageend = _page_poses[page + 1];

    return pageend - *page_begin;
}
