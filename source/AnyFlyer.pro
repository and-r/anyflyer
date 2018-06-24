TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH +=/usr/include/irrlicht
LIBS += -l Irrlicht

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += anyflyer_main.cpp \
    uavnode.cpp \
    settingsmgr.cpp \
    simulation_manager.cpp \
    dictionary.cpp \
    simeventreceiver.cpp \
    menulist.cpp \
    uavarray.cpp \
    camera.cpp

HEADERS += \
    uavnode.h \
    settingsmgr.h \
    simulation_manager.h \
    dictionary.h \
    simeventreceiver.h \
    secontainer.h \
    menulist.h \
    uavarray.h \
    camera.h \
    prompt.h
