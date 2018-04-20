#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T10:36:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ConfigManager
TEMPLATE = app

INCLUDEPATH = $$PWD/mxml-2.10/include/
LIBS = $$PWD/mxml-2.10/lib/libmxml.a


SOURCES += main.cpp\
        mainwindow.cpp \
    strutils.cpp \
    detector.cpp \
    xmlmanager.cpp

HEADERS  += mainwindow.h \
    strutils.h \
    common.h \
    detector.h \
    xmlmanager.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
