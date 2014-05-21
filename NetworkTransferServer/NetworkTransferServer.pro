#-------------------------------------------------
#
# Project created by QtCreator 2014-04-20T21:53:56
#
#-------------------------------------------------

QT       += core network

TARGET = NetworkTransferServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    tripserver.cpp \
    common.cpp \
    clientsocket.cpp \
    getopt.c

HEADERS += \
    tripserver.h \
    common.h \
    clientsocket.h \
    getopt.h
