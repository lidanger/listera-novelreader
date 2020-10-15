#include "bookbrowser.h"
#include "readerconfig.h"
#include "utils.h"

BookBrowser::BookBrowser(QString book_name)
    :_book_name(book_name)
{
    // 从设置中加载必要的信息
    if(_book_name.isEmpty())
        _book_name = ReaderConfig::Instance()->currentBook();

    auto file_path = ReaderConfig::Instance()->bookPath(_book_name);

    _current_page = ReaderConfig::Instance()->bookCurrentPage(_book_name);

    // 获取分页信息
    _bookPaging(file_path);
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

int BookBrowser::pageIndex(QString page_title)
{
    auto index = _page_titles.indexOf(page_title);
    if(index == -1)
        return 0;

    return index;
}

QString BookBrowser::pageContent(int page)
{
    if(_file == nullptr)
        return QString();

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return QString();
    }

    if(_page_titles.isEmpty())
    {
        auto content = _file->readAll();
        auto txt = _codec->toUnicode(content);

        _file->close();
        return txt;
    }

    if(page < 0 || page > _page_titles.size() - 1)
        return QString();

    auto pagebegin = _page_poses[_page_titles[page]];
    auto len = _getPageLength(page);

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

QString BookBrowser::pageContent(QString page_name)
{
    if(_file == nullptr)
        return QString();

    if(!_page_poses.contains(page_name))
        return QString();

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return QString();
    }

    auto pagebegin = _page_poses[page_name];
    auto len = _getPageLength(page_name);

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
    if(_file == nullptr)
        return QString();

    if(page < 0 || page > _page_titles.size() - 1)
        return QString();

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return QString();
    }

    auto pagebegin = _page_poses[_page_titles[page]];

    _file->seek(pagebegin);
    auto content = _file->read(num);

    return _codec->toUnicode(content);
}

QString BookBrowser::pagePreview(QString page_name, int num)
{
    if(_file == nullptr)
        return QString();

    if(!_page_poses.contains(page_name))
        return QString();

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return QString();
    }

    auto pagebegin = _page_poses[page_name];

    _file->seek(pagebegin);
    auto content = _file->read(num);

    return _codec->toUnicode(content);
}

bool BookBrowser::moveNext()
{
    if(_file == nullptr)
        return false;

    if(_current_page == _page_titles.size() - 1)
        return false;

    ++_current_page;
    ReaderConfig::Instance()->setBookCurrentPage(_book_name, _current_page);

    return true;
}

bool BookBrowser::movePrevious()
{
    if(_file == nullptr)
        return false;

    if(_current_page == 0)
        return false;

    --_current_page;
    ReaderConfig::Instance()->setBookCurrentPage(_book_name, _current_page);

    return true;
}

bool BookBrowser::moveToPage(int page)
{
    if(_file == nullptr)
        return false;

    if(page < 0 || page > _page_titles.size() - 1)
        return false;

    _current_page = page;
    ReaderConfig::Instance()->setBookCurrentPage(_book_name, _current_page);

    return true;
}

bool BookBrowser::moveToRatio(float ratio)
{
    if(_file == nullptr)
        return false;

    if(ratio < 0 || ratio > 1 || _page_titles.isEmpty())
        return false;

    auto page = (_page_titles.size() - 1) * ratio;
    _current_page = static_cast<int>(page);
    ReaderConfig::Instance()->setBookCurrentPage(_book_name, _current_page);

    return true;
}

void BookBrowser::_bookPaging(QString file_path)
{
    if(file_path.isEmpty())
        return;

    _file = new QFile(file_path);

    if(!_file->exists())
    {
        qDebug() << QObject::tr("Can not find book ") << _book_name << QObject::tr("'s corresponding file:") << _file->fileName() << endl;
        return;
    }

    _codec = GetCorrectTextCodec(_file->fileName());
    if(_codec == nullptr)
        return;

    if(!_file->open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    auto first_page_name = QObject::tr("First Page");
    _page_titles.push_back(first_page_name);
    _page_poses[first_page_name] = 0;

    do
    {
        auto pos = _file->pos();
        auto line = _file->readLine();
        auto txt = _codec->toUnicode(line).trimmed();

        if(is_toc_item(txt))
        {
            _page_titles.push_back(txt);
            _page_poses[txt] = pos;
        }

    } while(!_file->atEnd());

    _file->close();

    // 重新以只读模式打开
    _file->open(QIODevice::ReadOnly);
}

qint64 BookBrowser::_getPageLength(int page)
{
    auto pagebegin = _page_poses[_page_titles[page]];

    auto pageend = 9223372036854775807;
    if(page < _page_titles.size() - 1)
        pageend = _page_poses[_page_titles[page + 1]];

    return pageend - pagebegin;
}

qint64 BookBrowser::_getPageLength(QString page_title)
{
    auto pagebegin = _page_poses[page_title];

    auto page = _page_titles.indexOf(page_title);

    auto pageend = 9223372036854775807;
    if(page < _page_titles.size() - 1)
        pageend = _page_poses[_page_titles[page + 1]];

    return pageend - pagebegin;
}
