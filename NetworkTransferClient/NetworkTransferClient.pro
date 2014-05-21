#-------------------------------------------------
#
# Project created by QtCreator 2014-04-21T10:26:12
#
#-------------------------------------------------

QT       += core network

TARGET = NetworkTransferClient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    tripclient.cpp \
    common.cpp \
    getopt.c

HEADERS += \
    tripclient.h \
    common.h \
    getopt.h
