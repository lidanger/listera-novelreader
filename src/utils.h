#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QTextCodec>
#include <QDebug>
#include <QFile>
#include <QSet>

// 获取字符编码
QTextCodec* GetCorrectTextCodec(const QByteArray &ba);

// 获取字符编码
QTextCodec* GetCorrectTextCodec(const QString &file_path, int check_length = 100);

// 字符是否十进制数字
bool is_digit(QChar aa);

// 字符串是否小说目录项
bool is_toc_item(const QString &line);

// 打开浏览器执行搜索
void startWebSearch(const QString &text, const QString &engine = QString());
QStringList searchEngines();
QString searchEngineChineseName(const QString &engine);
QString serachEngineName(const QString &chineseName);

void startWebTranslate(const QString &text, const QString &engine = QString());
QStringList translateEngines();
QString translateEngineChineseName(const QString &engine);
QString translateEngineName(const QString &chineseName);

#endif // UTILS_H
