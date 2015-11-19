#-------------------------------------------------
#
# Project created by QtCreator 2015-11-18T12:28:45
#
#-------------------------------------------------

#CONFIG += link_pkgconfig
#PKGCONFIG += Qt5SerialPort

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = smooth
TEMPLATE = app


SOURCES +=  src/main.cpp \
            src/mainwindow.cpp \
            src/bat6window.cpp \
            src/console.cpp

HEADERS  += include/mainwindow.h \
            include/bat6window.h \
            include/console.h

FORMS    += forms/mainwindow.ui \
            forms/bat6window.ui

INCLUDEPATH = include
