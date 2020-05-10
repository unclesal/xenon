########################################################################################################################
#                                           Assembling Xenon Desktop application                                       #
#                                                                                                                      #
# Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                           Created 16 mar 2019 at 13:08 #
########################################################################################################################

TEMPLATE=app
TARGET=build/xedesktop

MOC_DIR=build/moc
UI_DIR=build/uic
RCC_DIR=build/obj
OBJECTS_DIR=build/obj

QT += core gui widgets network uitools
CONFIG   += qt warn_on thread rtti exceptions

# CONFIG -= debug
# CONFIG += release

CONFIG += debug

QMAKE_CXXFLAGS += -std=c++11 -fpermissive -fPIC -O2 -pipe

#LIBS += 
#-lnvidia-ml -lcudart -L/opt/cuda/lib64 libXNVCtrl/libXNVCtrl.a -lX11 -lXext -lpci -lcryptopp -lgdbm

INCLUDEPATH += ../common ../common/network ../common/widgets ../3dparty

FORMS +=../common/widgets/lcd_with_buttons.ui   \
        ../common/widgets/control_widget.ui     \
        main_window.ui

HEADERS+=../common/Settings.h                   \
        ../common/JSONAble.h                    \
        ../common/network/Aircraft.h            \
        ../common/network/Ambient.h             \
        ../common/network/Autopilot.h           \
        ../common/network/XeNetwork.h           \
        ../common/widgets/RepeatableButton.h    \
        ../common/widgets/LCDWithButtons.h      \
        ../common/widgets/ControlWidget.h       \
        XeDesktop.h


SOURCES+=../common/JSONAble.cpp                 \
        ../common/network/Aircraft.cpp          \
        ../common/network/Ambient.cpp           \
        ../common/network/Autopilot.cpp         \
        ../common/network/XeNetwork.cpp         \
        ../common/widgets/RepeatableButton.cpp  \
        ../common/widgets/LCDWithButtons.cpp    \
        ../common/widgets/ControlWidget.cpp     \
        XeDesktop.cpp
        
RESOURCES += resources/MainTab.css

