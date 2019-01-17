#-------------------------------------------------
#
# Project created by QtCreator 2018-07-23T19:13:00
#
#-------------------------------------------------
INCLUDEPATH += E:\app_udp\eigen
OBJECTS_DIR  = tmp
MOC_DIR      = tmp

QT       += sql

QT       += core gui

QT       += charts

QT       += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Serial_Communication1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES +=  \
        main.cpp \
        mainwindow.cpp \
    datacenter.cpp \
    matlib.cpp \
    chart.cpp \
    chartview.cpp \
    dialog_setting_1.cpp \
    dialog_setting_2.cpp \
    qwcomboboxdelegate.cpp \
    label_mine.cpp \
    udp_json_api.cpp \
    kalman.cpp \
    time_thread.cpp

HEADERS += \
        mainwindow.h \
    datacenter.h \
    matlib.h \
    chart.h \
    chartview.h \
    dialog_setting_1.h \
    dialog_setting_2.h \
    qwcomboboxdelegate.h \
    label_mine.h \
    udp_json_api.h \
    kalman.h \
    time_thread.h

FORMS += \
    mainwindow.ui \
    dialog_setting_1.ui \
    dialog_setting_2.ui

RESOURCES += \
    images.qrc

RC_ICONS = soccer.ico

