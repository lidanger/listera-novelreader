#ifndef TEXTCONTENT_H
#define TEXTCONTENT_H

#include <QTextEdit>
#include <QMenu>

class TextContent : public QTextEdit
{
    Q_OBJECT

public:
    TextContent(QWidget *parent = nullptr);

private:
    void init_ui();

private slots:
    void _customContextMenuRequested(const QPoint &pos);
    void _web_search();
    void _translate();

private:
    bool eventFilter(QObject *target, QEvent *event);

public slots:
    void refresh();

    void setFont(const QFont &f);
    QFont font() { return _font; }
    void setFontFamily(const QString &fontFamily);
    QString fontFamily() { return _font.family(); }
    void setFontItalic(bool italic);
    bool fontItalic() { return _font.italic(); }
    void setFontPointSize(qreal s);
    qreal fontPointSize() { return _font.pointSizeF(); }
    void setFontUnderline(bool underline);
    bool fontUnderline() { return _font.underline(); }
    void setFontWeight(int w);
    int fontWeight() { return _font.weight(); }
    void setPlainText(const QString &text);
    QString plainText() { return _origin_text; }
    void setText(const QString &text);
    QString text() { return _origin_text; }
    void setTextColor(const QColor &c);
    QColor textColor() { return _text_color; }

    void setBackgroundColor(const QColor &c);
    QColor backgroundColor() { return _background_color; }
    void setBackgroundImage(const QString &file_path);
    QString backgroundImage() { return _background_image_file; }
    void setLineSpacing(double spacing);
    double lineSpacing() { return _line_space; }
    void setCharSpacing(double spacing);
    double charSpacing() { return _char_space; }
    void setParagraphSpaceing(int spacing);
    int paragraphSpaceing() { return _para_space; }
    void setRemoveEmptyLine(bool remove);
    bool removeEmptyLine() { return _remove_empty_line; }
    void setDarkMode(bool dark);
    bool darkMode() { return _darkmode; }

private:
    QMenu *_menu;

    QFont _font;
    QColor _text_color;
    QColor _background_color;
    QString _background_image_file;
    double _line_space = 1;
    int _para_space = 0;
    double _char_space = 2;
    bool _remove_empty_line = false;
    bool _darkmode = false;

    QString _origin_text;
};

#endif // TEXTCONTENT_H
