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
    pdp8_utilities.cpp \
    numberarea.cpp \
    codeeditor.cpp \
    pdp8dissmwindow.cpp \
    pdp8dissam.cpp \
    pdp8state.cpp \
    timer.cpp \
    tty_vt52.cpp \
    qtty.cpp \
    placabletextedit.cpp \
    qtty2.cpp \
    terminal.cpp \
    termina_vt52.cpp \
    pdp8events.cpp \
    wiretest.cpp \
    pdp8_rk05.cpp \
    pdp8_rf08.cpp \
    pdp11_cpu.cpp

HEADERS  += mainwindow.h \
    panelswitch.h \
    switchrow.h \
    octalvalidator.h \
    console.h \
    pdp8i.h \
    pdp8_utilities.h \
    numberarea.h \
    codeeditor.h \
    pdp8dissmwindow.h \
    pdp8dissam.h \
    pdp8state.h \
    timer.h \
    includes.h \
    tty_vt52.h \
    qtty.h \
    placabletextedit.h \
    qtty2.h \
    terminal.h \
    termina_vt52.h \
    pdp8events.h \
    wiretest.h \
    pdp8_rk05.h \
    pdp8_rf08.h \
    pdp11_cpu.h

FORMS    += mainwindow.ui


DISTFILES += \
    TELETYPE1945-1985.ttf

RESOURCES += \
    pdp8emulator.qrc
