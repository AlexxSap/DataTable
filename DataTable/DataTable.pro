TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread -lgtest

SOURCES += \
        DataTable.cpp \
        main.cpp

HEADERS += \
    DataTable.h
