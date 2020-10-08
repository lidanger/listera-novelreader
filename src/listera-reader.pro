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
    readerconfig.cpp \
    bookcontents.cpp \
    bookbrowser.cpp \
    utils.cpp \
    readersettings.cpp \
    bookmarks.cpp

RESOURCES +=         ../resources/resources.qrc

TRANSLATIONS = ../translations/listera-novelreader_zh_CN.ts

HEADERS += \
    mainwindow.h \
    application.h \
    textcontent.h \
    readerconfig.h \
    bookcontents.h \
    utils.h \
    bookbrowser.h \
    readersettings.h \
    bookmarks.h
