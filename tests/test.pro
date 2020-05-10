TEMPLATE=app
TARGET=build/test

MOC_DIR=build/moc
UI_DIR=build/uic
RCC_DIR=build/obj
OBJECTS_DIR=build/obj

QT += core gui widgets network
CONFIG   += qt warn_on thread rtti exceptions

# CONFIG -= debug
# CONFIG += release

CONFIG += debug

QMAKE_CXXFLAGS += -std=c++11 -fpermissive -fPIC -O2 -pipe

INCLUDEPATH += ../common ../common/network

HEADERS+=

SOURCES+=./main.cpp

RESOURCES +=


