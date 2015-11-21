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

unix:!macx: LIBS += -lqwtplot3d -lGLU
unix: QMAKE_CXXFLAGS += -std=c++11

SOURCES +=  main.cpp \
            widgets/mainwindow.cpp \
            widgets/cellwidget.cpp \
            widgets/consolewidget.cpp \
            widgets/bat6widget.cpp \
            models/pvmodel.cpp \
    widgets/plotwidget.cpp

HEADERS  += widgets/mainwindow.h \
            widgets/cellwidget.h \
            widgets/consolewidget.h \
            widgets/bat6widget.h \
            models/pvmodel.h \
    widgets/plotwidget.h

FORMS    += mainwindow.ui \
            bat6widget.ui \
            cellwidget.ui