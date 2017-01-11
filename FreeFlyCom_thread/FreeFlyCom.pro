#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T22:49:55
#
#-------------------------------------------------

QT       += core gui
QT       += serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FreeFlyCom
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    mysendthread.cpp \
    myrecvthread.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    mysendthread.h \
    myrecvthread.h

FORMS    += mainwindow.ui \
    aboutdialog.ui

RESOURCES += \
    images.qrc

DISTFILES += \
    FreeFlyCom.rc

RC_FILE = FreeFlyCom.rc
