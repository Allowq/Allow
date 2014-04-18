#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T12:38:04
#
#-------------------------------------------------

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += network

TARGET = NetworkTransferClient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    networktransfer.cpp \
    clientthread.cpp \
    common.cpp

HEADERS += \
    common.h \
    networktransfer.h \
    clientthread.h \
    networkthread.h
