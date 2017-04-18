#-------------------------------------------------
#
# Project created by QtCreator 2016-08-26T14:17:41
#
#-------------------------------------------------

QT       += core gui serialport printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NOVATesting
TEMPLATE = app

CONFIG += console

SOURCES += main.cpp\
        mainwindow.cpp \
    usrpswdialog.cpp

HEADERS  += mainwindow.h \
    usrpswdialog.h

FORMS    += mainwindow.ui \
    usrpswdialog.ui

RESOURCES += \
    logo.qrc \
    logo.qrc

DISTFILES +=
