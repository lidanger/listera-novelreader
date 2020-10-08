#ifndef SHOTCUTSCONFIG_H
#define SHOTCUTSCONFIG_H

#include <DDialog>
#include <QListWidget>

DWIDGET_USE_NAMESPACE

class ReaderSettings : public DDialog
{
    Q_OBJECT

public:
    ReaderSettings(QWidget *parent = nullptr);

private:
    void init_ui();
    void init_data();

private slots:
    void onButtonClicked(int index, const QString &text);

private:
    QListWidget *_shortcut_list;
};

#endif // SHOTCUTSCONFIG_H
