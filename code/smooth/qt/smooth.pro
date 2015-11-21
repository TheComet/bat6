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
CONFIG += c++11


SOURCES +=  main.cpp \
            mainwindow.cpp \
            bat6window.cpp \
            console.cpp \
            cell.cpp \
    pvmodel.cpp

HEADERS  += mainwindow.h \
            bat6window.h \
            console.h \
            cell.h \
    pvmodel.h

FORMS    += mainwindow.ui \
            bat6window.ui \
            cell.ui
