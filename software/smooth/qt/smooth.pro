#-------------------------------------------------
#
# Project created by QtCreator 2015-11-18T12:28:45
#
#-------------------------------------------------

#CONFIG += link_pkgconfig
#PKGCONFIG += Qt5SerialPort

QT       += core widgets gui printsupport opengl

TARGET = smooth
TEMPLATE = app
CONFIG += c++11

unix:!macx: LIBS += -lqwt6 -lqwtplot3d -lGLU
unix: QMAKE_CXXFLAGS += -std=c++11

SOURCES +=  main.cpp \
            widgets/mainwindow.cpp \
            widgets/cellwidget.cpp \
            widgets/consolewidget.cpp \
            widgets/bat6widget.cpp \
            widgets/characteristiccurve2dwidget.cpp \
            models/pvcell.cpp \
            models/pvchain.cpp \
            models/pvarray.cpp \
    widgets/characteristiccurve3dwidget.cpp

HEADERS  += widgets/mainwindow.h \
            widgets/cellwidget.h \
            widgets/consolewidget.h \
            widgets/bat6widget.h \
            widgets/characteristiccurve2dwidget.h \
            models/pvcell.h \
            models/pvchain.h \
            models/pvarray.h \
    widgets/characteristiccurve3dwidget.h

FORMS    += mainwindow.ui \
            bat6widget.ui \
            cellwidget.ui
