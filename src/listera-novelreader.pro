QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = listera-novelreader
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    application.cpp \
    readerconfig.cpp \
    bookcontents.cpp \
    bookbrowser.cpp \
    utils.cpp \
    bookmarks.cpp \
    dsettingsbuilder.cpp \
    shotcutsettings.cpp \
    textcontent.cpp

RESOURCES +=         ../resources/resources.qrc

TRANSLATIONS = ../translations/listera-novelreader_zh_CN.ts

HEADERS += \
    mainwindow.h \
    application.h \
    readerconfig.h \
    bookcontents.h \
    utils.h \
    bookbrowser.h \
    bookmarks.h \
    dsettingsbuilder.h \
    shotcutsettings.h \
    textcontent.h
