#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QSettings>
#include <QSize>
#include <QString>

// 忽略 GNU 语句表达式扩展警告
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"

class ReaderConfig
{
#define _getValue(groupname, keyname, defaultValue)                               \
    ({                                                                            \
        QSettings sets(_org_name, _app_name);                                     \
        sets.beginGroup(groupname);                                               \
        sets.value(keyname, defaultValue);                                                      \
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

#define _removeKey(groupname, keyname)                                            \
    ({                                                                            \
        QSettings sets(_org_name, _app_name);                                     \
        sets.beginGroup(groupname);                                               \
        sets.remove(keyname);                                                     \
        sets.endGroup();                                                          \
    })

public:
    QVariant getValue(QString groupname, QString keyname, const QVariant &defaultValue = QVariant()) { return _getValue(groupname, keyname, defaultValue); }
    QStringList getKeys(QString groupname) { return _getKeys(groupname); }
    void setValue(QString groupname, QString keyname, QVariant value) { _setValue(groupname, keyname, value); }
    void removeKey(QString groupname, QString keyname) { _removeKey(groupname, keyname); }

public:
    static ReaderConfig *Instance() { return _inst == nullptr ? new ReaderConfig() : _inst; }

public:
    // 书库
    void addBook(QString book_path);
    inline QStringList bookNames() { return _getKeys("Library"); }
    inline QString bookPath(QString book_name) { return _getValue("Library", book_name, "").toString(); }
    inline void removeBook(QString book_name) { _removeKey("Library", book_name); }
    inline void clearBooks() { _removeKey("Library", ""); }

    // 书籍阅读
    void setCurrentBook(QString book_name);
    inline QString currentBook() { return _getValue("Reading", "CurrentBook", "").toString(); }

    inline void setBookCurrentPage(QString book_name, int page) { _setValue("Reading", book_name, page); }
    inline int bookCurrentPage(QString book_name) { return _getValue("Reading", book_name, 0).toInt(); }

    inline QStringList readingHistory() { return _getKeys("History"); }
    inline QString readingHistoryItem(QString item_name) { return _getValue("History", item_name, "").toString(); }
    inline void clearHistory() { _removeKey("History", ""); }

    void addBookmark(QString book_name, int page, QString page_title);
    inline QStringList bookmarks() { return _getKeys("Bookmark"); }
    inline QString bookmarkTime(QString key) { return _getValue("Bookmark", key, "").toString(); }
    inline void removeBookmark(QString key) { _removeKey("Bookmark", key); }
    inline void clearBookmarks() { _removeKey("Bookmark", ""); }

    // 窗口状态
    inline void setWindowState(int state) { _setValue("Window", "State", state); }
    inline int windowState() { return _getValue("Window", "State", 0).toInt(); }

    void setWindowSize(QSize size);
    QSize windowSize();

    inline void setSidebarState(int state) { _setValue("Window", "SidebarState", state); }
    inline int sidebarState() { return _getValue("Window", "SidebarState", 1).toInt(); }

    inline void setSidebarWidth(int width) { _setValue("Window", "SidebarWidth", width); }
    inline int sidebarWidth() { return _getValue("Window", "SidebarWidth", 200).toInt(); }

    inline void setToolbarState(int state) { _setValue("Window", "ToolbarState", state); }
    inline int toolbarState() { return _getValue("Window", "ToolbarState", 0).toInt(); }

    inline void setStatusbarState(int state) { _setValue("Window", "StatusbarState", state); }
    inline int statusbarState() { return _getValue("Window", "StatusbarState", 1).toInt(); }

    // 阅读视图状态
    inline void setFontFamily(QString font_name) { _setValue("Reading", "FontFamily", font_name); }
    inline QString fontFamily() { return _getValue("Reading", "FontFamily", "").toString(); }

    inline void setFontSize(double font_size) { _setValue("Reading", "FontSize", font_size); }
    inline double fontSize() { return _getValue("Reading", "FontSize", 12).toDouble(); }

    void setTextColor(QColor color);
    QColor textColor();

    inline void setCharacterSpace(double space) { _setValue("Reading", "CharacterSpace", space); }
    inline double characterSpace() { return _getValue("Reading", "CharacterSpace", 1).toDouble(); }

    inline void setLineSpace(double space) { _setValue("Reading", "LineSpace", space); }
    inline double lineSpace() { return _getValue("Reading", "LineSpace", 1.1).toDouble(); }

    inline void setParagraphSpace(int space) { _setValue("Reading", "ParagraphSpace", space); }
    inline int paragraphSpace() { return _getValue("Reading", "ParagraphSpace", 2).toInt(); }

    void setBackgroundColor(QColor color);
    QColor backgroundColor();
    void setBackgroundPicture(QString file_path);
    QString backgroundPicture() { return _getValue("Reading", "BackgroundPicture", "").toString(); }

    inline void setDarkMode(bool state) { _setValue("Reading", "DarkMode", state); }
    inline bool darkMode() { return _getValue("Reading", "DarkMode", false).toBool(); }

    inline void setRemoveEmptyLine(bool remove) { _setValue("Reading", "RemoveEmptyLine", remove); }
    inline bool removeEmptyLine() { return _getValue("Reading", "RemoveEmptyLine", false).toBool(); }

    // 快捷键
    inline void setShortcut(QString function_name, QString keys) { _setValue("Shotcuts", function_name, keys); }
    inline QString shortcutKeys(QString function_name) { return _getValue("Shotcuts", function_name, "").toString(); }

    inline void setSearchEngine(QString engine) { _setValue("Web", "SearchEngine", engine); }
    inline QString searchEngine() { return _getValue("Web", "SearchEngine", "Baidu").toString(); }

    inline void setTranslateEngine(QString engine) { _setValue("Web", "TranslateEngine", engine); }
    inline QString translateEngine() { return _getValue("Web", "TranslateEngine", "Youdao").toString(); }

    // 从本地格式配置文件中得到符合 DSettings 的 JSON 字符串
    QByteArray getDSettingsJson();

private:
    ReaderConfig();

private:
    static ReaderConfig *_inst;

private:
    QString _org_name;
    QString _app_name;
};

#endif // APPCONFIG_H
