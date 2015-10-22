#-------------------------------------------------
#
# Project created by QtCreator 2015-10-07T14:48:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PDP8EmulatorTest
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    panelswitch.cpp \
    switchrow.cpp \
    octalvalidator.cpp \
    console.cpp \
    pdp8i.cpp \
    pdp8_utilities.cpp

HEADERS  += mainwindow.h \
    panelswitch.h \
    switchrow.h \
    octalvalidator.h \
    console.h \
    pdp8i.h \
    pdp8_utilities.h

FORMS    += mainwindow.ui
include(panelswitch.pri)
