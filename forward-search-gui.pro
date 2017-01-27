#-------------------------------------------------
#
# Project created by QtCreator 2016-10-30T00:43:17
#
#-------------------------------------------------

QT     += core gui
CONFIG += c++11
CONFIG += object_parallel_to_source

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = forward-search-gui
TEMPLATE = app

LIBS += -lboost_thread -lboost_system -lboost_chrono

SOURCES += main.cpp\
        mainwindow.cpp \
    graphitem.cpp \
    graphsearch.cpp \
    breadthfirstsearch.cpp \
    fielditem.cpp \
    robotmap.cpp \
    astarsearch.cpp \
    thetastarsearch.cpp

HEADERS  += mainwindow.h \
    graphitem.h \
    graphsearch.h \
    breadthfirstsearch.h \
    fielditem.h \
    robotmap.h \
    astarsearch.h \
    thetastarsearch.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS_DEBUG -= -O -O1
QMAKE_CXXFLAGS_DEBUG *= -O2
#QMAKE_CXXFLAGS_DEBUG += -v -msse

QMAKE_CXXFLAGS_RELEASE -= -O -O1 -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
#QMAKE_CXXFLAGS_RELEASE += -mtune=native -march=native
