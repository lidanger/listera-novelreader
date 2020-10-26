#include <QString>
#include <QTextCodec>
#include <QDebug>
#include <QFile>
#include <QSet>
#include <QDesktopServices>
#include <QUrl>

// 编码转换
QString GetCorrectUnicode(const QByteArray &ba)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode(ba.constData(), ba.size(), &state);

    qDebug() << state.invalidChars << endl;

    // GBK
    if (state.invalidChars == 4)
    {
        text = QTextCodec::codecForName("GBK")->toUnicode(ba);
    }
    else if(state.invalidChars == 3)    // unicode
    {
        text = QTextCodec::codecForName("Unicode")->toUnicode(ba);
    }

    return text;
}

QString GetCorrectText(const QByteArray &ba)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode(ba.constData(), ba.size(), &state);

    if (state.invalidChars > 0)
    {
        text = QTextCodec::codecForName("GBK")->toUnicode(ba);
    }

    return text;
}

QTextCodec* GetCorrectTextCodec(const QByteArray &ba)
{
    QTextCodec::ConverterState state;
    auto *codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode(ba.constData(), ba.size(), &state);

    if (state.invalidChars > 0)
    {
        return QTextCodec::codecForName("GBK");
    }

    return codec;
}

QTextCodec* GetCorrectTextCodec(const QString &file_path, int check_length)
{
    QFile file(file_path);
    if(!file.exists())
    {
        qDebug() <<  QString("以下文件未找到:\n%1").arg(file_path) << endl;
        return nullptr;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        return nullptr;
    }

    auto content = file.read(check_length);
    file.close();

    QTextCodec::ConverterState state;
    auto *codec = QTextCodec::codecForName("UTF-8");
    auto text = codec->toUnicode(content.constData(), content.size(), &state);

    if (state.invalidChars > 0)
    {
        return QTextCodec::codecForName("GBK");
    }

    return codec;
}

static QSet<QChar> _zh_nums({ u'零',
                             u'一', u'二', u'三', u'四', u'五', u'六', u'七', u'八', u'九',
                             u'十', u'百', u'千', u'万', u'亿',
                             u'壹', u'贰', u'叁', u'肆', u'伍', u'陆', u'柒', u'捌', u'玖',
                             u'拾', u'佰', u'仟' });

// 字符是否十进制数字
bool is_digit(QChar aa)
{
    // 汉字
    if(_zh_nums.contains(aa))
        return true;

    // 其他
    return aa.isDigit();
}

static QChar _toc_begin = u'第';
static QSet<QChar> _toc_filters({ u'章', u'卷', u'回', u'部', u'节', u'集', u'篇' });

// 字符串是否小说目录项
bool is_toc_item(const QString &line)
{
    //line = line.trimmed();

    if(line.length() > 50)
        return false;

    if(line[0] != _toc_begin)
        return false;

    for(int i = 1; i < 12 && i < line.length(); i++)
    {
        auto aa = line[i];

        if(aa.isSpace())
            continue;

        if(_toc_filters.contains(aa))
            return true;

        if(!is_digit(aa))
            break;
    }

    return false;
}

static QMap<QString, QString> _web_search_engine(
{
            { "Baidu", "https://www.baidu.com/s?wd=%1" },
            { "Google", "https://www.google.com.hk/search?q=%1" },
            { "Bing", "https://bing.com/search?q=%1" },
            { "Sogou", "https://www.sogou.com/sogou?query=%1" },
});
static QMap<QString, QString> _web_search_engine_names(
{
            { "百度", "Baidu" },
            { "谷歌", "Google" },
            { "必应", "Bing" },
            { "搜狗", "Sogou" },
});

void startWebSearch(const QString &text, const QString &engine)
{
    if(text.isEmpty())
        return;

    QString engine_name = engine;
    if(engine_name.isEmpty())
        engine_name = "baidu";

    if(!_web_search_engine.contains(engine_name))
    {
        if(_web_search_engine_names.contains(engine_name))
            engine_name = _web_search_engine_names[engine_name];
        else
            return;
    }

    auto search_url = _web_search_engine[engine_name].arg(text);
    QDesktopServices::openUrl(QUrl(search_url));
}

QStringList searchEngines()
{
    return _web_search_engine.keys();
}

QString searchEngineChineseName(const QString &engine)
{
    for(auto it = _web_search_engine_names.begin(); it != _web_search_engine_names.end(); it++)
    {
        if(it.value() == engine)
            return it.key();
    }

    return QString();
}

QString serachEngineName(const QString &chineseName)
{
    if(_web_search_engine_names.contains(chineseName))
        return _web_search_engine_names[chineseName];

    return QString();
}

static QMap<QString, QString> _web_translate_engine(
{
            { "Youdao", "http://www.youdao.com/w/%1" },
            { "Baidu", "https://fanyi.baidu.com/?aldtype=85#zh/en/%1"},
            { "Google", "https://translate.google.cn/#view=home&op=translate&sl=auto&tl=en&text=%1" }
});
static QMap<QString, QString> _web_translate_engine_names(
{
            { "有道", "Youdao" },
            { "百度", "Baidu" },
            { "谷歌", "Google" }
});

void startWebTranslate(const QString &text, const QString &engine)
{
    if(text.isEmpty())
        return;

    QString engine_name = engine;
    if(engine_name.isEmpty())
        engine_name = "Baidu";

    if(!_web_translate_engine.contains(engine_name))
    {
        if(_web_translate_engine_names.contains(engine_name))
            engine_name = _web_translate_engine_names[engine_name];
        else
            return;
    }

    auto search_url = _web_translate_engine[engine_name].arg(text);
    QDesktopServices::openUrl(QUrl(search_url));
}

QStringList translateEngines()
{
    return _web_translate_engine.keys();
}

QString translateEngineChineseName(const QString &engine)
{
    for(auto it = _web_translate_engine_names.begin(); it != _web_translate_engine_names.end(); it++)
    {
        if(it.value() == engine)
            return it.key();
    }

    return QString();
}

QString translateEngineName(const QString &chineseName)
{
    if(_web_translate_engine_names.contains(chineseName))
        return _web_translate_engine_names[chineseName];

    return QString();
}
