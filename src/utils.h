#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QTextCodec>
#include <QDebug>
#include <QFile>
#include <QSet>

// 编码转换
QString GetCorrectUnicode(const QByteArray &ba);

QString GetCorrectText(const QByteArray &ba);

QTextCodec* GetCorrectTextCodec(const QByteArray &ba);

QTextCodec* GetCorrectTextCodec(QString file_path, int check_length = 100);

// 字符是否十进制数字
bool is_digit(QChar aa);

// 字符串是否小说目录项
bool is_toc_item(QString line);

#endif // UTILS_H
