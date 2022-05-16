QT += core
QT -= gui

TARGET = ServerClient
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

win32:LIBS += -lws2_32

TEMPLATE = app

SOURCES	+= *.cpp
HEADERS += *.h \
    ErrorCodes.h
