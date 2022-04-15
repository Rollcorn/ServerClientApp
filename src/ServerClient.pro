QT += core
QT -= gui

TARGET = ServerClient
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

win32:LIBS += -lws2_32

TEMPLATE = app

SOURCES	+= *.cpp \
    c_tcpsocket.cpp
HEADERS += *.h \
    ErrorCodes.h \
    Config.h \
    c_tcpsocket.h
