QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = listera-reader
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    application.cpp \
    textcontent.cpp \
    readerconfig.cpp

RESOURCES +=         resources.qrc

TRANSLATIONS = listera-reader_zh_CN.ts

HEADERS += \
    mainwindow.h \
    application.h \
    textcontent.h \
    readerconfig.h
