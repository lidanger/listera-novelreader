#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QSettings>
#include <QSize>
#include <QString>

// 忽略 GNU 语句表达式扩展警告
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"

class ReaderConfig
{
#define _getValue(groupname, keyname)                                             \
    ({                                                                            \
        QSettings sets(_org_name, _app_name);                                     \
        sets.beginGroup(groupname);                                               \
        sets.value(keyname);                                                      \
    })

#define _getKeys(groupname)                                                       \
    ({                                                                            \
        QSettings sets(_org_name, _app_name);                                     \
        sets.beginGroup(groupname);                                               \
        sets.childKeys();                                                         \
    })

#define _setValue(groupname, keyname, value)                                      \
    ({                                                                            \
        QSettings sets(_org_name, _app_name);                                     \
        sets.beginGroup(groupname);                                               \
        sets.setValue(keyname, value);                                            \
        sets.endGroup();                                                          \
    })

public:
    static ReaderConfig *Instance() { return _inst == nullptr ? new ReaderConfig() : _inst; }

public:
    // 书籍阅读
    void addBook(QString book_path);
    inline QStringList bookNames() { return _getKeys("Library"); }
    inline QString bookPath(QString book_name) { return _getValue("Library", book_name).toString(); }

    void setCurrentBook(QString book_name);
    inline QString currentBook() { return _getValue("Reading", "CurrentBook").toString(); }

    inline void setBookCurrentPage(QString book_name, int page) { _setValue("Reading", book_name, page); }
    inline int bookCurrentPage(QString book_name) { return _getValue("Reading", book_name).toInt(); }

    inline QStringList readingHistory() { return _getKeys("History"); }
    inline QString readingHistoryItem(QString item_name) { return _getValue("History", item_name).toString(); }

    void addBookmark(QString book_name, int page);
    inline QStringList bookmarks() { return _getKeys("Bookmark"); }

    // 窗口状态
    inline void setWindowState(int state) { _setValue("Window", "State", state); }
    inline int windowState() { return _getValue("Window", "State").toInt(); }

    inline void setWindowSize(QSize size)
    {
        _setValue("Window", "Width", size.width());
        _setValue("Window", "Height", size.height());
    }
    inline QSize windowSize() { return QSize(_getValue("Window", "Width").toInt(), _getValue("Window", "Height").toInt()); }

    inline void setSidebarState(int state) { _setValue("Window", "SidebarState", state); }
    inline int sidebarState() { return _getValue("Window", "SidebarState").toInt(); }

    inline void setSidebarWidth(int width) { _setValue("Window", "SidebarWidth", width); }
    inline int sidebarWidth() { return _getValue("Window", "SidebarWidth").toInt(); }

    inline void setToolbarState(int state) { _setValue("Window", "ToolbarState", state); }
    inline int toolbarState() { return _getValue("Window", "ToolbarState").toInt(); }

    // 阅读视图状态
    inline void setReadingFont(QString font_name) { _setValue("Reading", "FontName", font_name); }
    inline QString readingFont() { return _getValue("Reading", "FontName").toString(); }

    inline void setReadingFontSize(int font_size) { _setValue("Reading", "FontSize", font_size); }
    inline int readingFontSize() { return _getValue("Reading", "FontSize").toInt(); }

    inline void setReadingCharacterSpace(int space) { _setValue("Reading", "CharacterSpace", space); }
    inline int readingCharacterSpace() { return _getValue("Reading", "CharacterSpace").toInt(); }

    inline void setReadingLineSpace(int space) { _setValue("Reading", "LineSpace", space); }
    inline int readingLineSpace() { return _getValue("Reading", "LineSpace").toInt(); }

    inline void setRedingParagraphSpace(int space) { _setValue("Reading", "ParagraphSpace", space); }
    inline int readingParagraphSpace() { return _getValue("Reading", "ParagraphSpace").toInt(); }

    inline void setReadingBackgroundColor(QString color_name) { _setValue("Reading", "BackgroundColor", color_name); }
    inline QString readingBackgroundColor() { return _getValue("Reading", "BackgroundColor").toString(); }
    void setReadingBackgroundPicture(QString file_path);
    inline QString readingBackgroundPicture() { return _getValue("Reading", "BackgroundPicture").toString(); }

    inline void setReadingDarkMode(int state) { _setValue("Reading", "DarkMode", state); }
    inline int readingDarkMode() { return _getValue("Reading", "DarkMode").toInt(); }

private:
    ReaderConfig();

private:
    static ReaderConfig *_inst;

private:
    QString _org_name;
    QString _app_name;
};

#endif // APPCONFIG_H
