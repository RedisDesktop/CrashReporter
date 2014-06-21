#-------------------------------------------------
#
# Project created by QtCreator 2014-06-21T13:16:40
#
#-------------------------------------------------

QT       += widgets network testlib

TARGET = tests
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_crashreportertest.cpp \
    $$PWD/../../src/CrashReporter.cpp

HEADERS +=  $$PWD/../../src/*.h

FORMS += $$PWD/../../ui/*.ui

INCLUDEPATH += $$PWD/../../src/

DEFINES += SRCDIR=\\\"$$PWD/\\\"

DESTDIR = $$PWD/../bin
