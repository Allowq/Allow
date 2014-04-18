#-------------------------------------------------
#
# Project created by QtCreator 2014-02-16T10:08:39
#
#-------------------------------------------------

QT       += core  network gui widgets

TARGET = NetworkTransferServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    common.cpp \
    networktransfer.cpp \
    serverthread.cpp

HEADERS += \
    common.h \
    networktransfer.h \
    serverthread.h \
    networkthread.h
