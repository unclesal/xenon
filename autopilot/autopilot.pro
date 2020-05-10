########################################################################################################################
#                                          Assembling Xenon Autopilot application                                      #
#                                                                                                                      #
# Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                           Created 20 sep 2019 at 18:54 #
########################################################################################################################

TEMPLATE=app
TARGET=build/xenon_autopilot

MOC_DIR=build/moc
UI_DIR=build/uic
RCC_DIR=build/obj
OBJECTS_DIR=build/obj

QT += core gui widgets network uitools
CONFIG   += qt warn_on thread rtti exceptions

CONFIG -= debug
CONFIG += release

QMAKE_CXXFLAGS += -std=c++11 -fpermissive -fPIC -O2 -pipe

#LIBS += 
#-lnvidia-ml -lcudart -L/opt/cuda/lib64 libXNVCtrl/libXNVCtrl.a -lX11 -lXext -lpci -lcryptopp -lgdbm

INCLUDEPATH += ../common ../common/network ../common/widgets ../3dparty

# Флаг, показывающий, что мы находимся в некоем внешнем установщике, штуке, которая сидит
# снаружи симулятора и может устанавливать по сети какие-то переменные в симуляторе.
DEFINES += EXTERNAL_SETTER

# Везде, кроме андроида.
DEFINES += UX_DESKTOP

#linux:!android {
#    message("* Using settings for Unix/Linux.")
#    # LIBS += -L/path/to/linux/libs
#}

android {
    message("* Using settings for Android.")
    DEFINES -= UX_DESKTOP
    DEFINES += UX_ANDROID
    # LIBS += -L/path/to/android/libs
}

FORMS +=../common/widgets/lcd_with_buttons.ui           \
        ../common/widgets/control_widget.ui

HEADERS+=../common/Settings.h                           \
        ../common/UserInterface.h                       \
        ../common/JSONAble.h                            \
        ../common/network/AircraftState.h               \
        ../common/network/Ambient.h                     \
        ../common/network/CommandAutopilotState.h       \
        ../common/network/CommandSet.h                  \
        ../common/network/XeNetwork.h                   \
        ../common/widgets/RepeatableButton.h            \
        ../common/widgets/LCDWithButtons.h              \
        ../common/widgets/AutopilotControlWidget.h      \
        XeAutopilot.h


SOURCES+=../common/JSONAble.cpp                         \
        ../common/network/AircraftState.cpp             \
        ../common/network/Ambient.cpp                   \
        ../common/network/CommandAutopilotState.cpp     \
        ../common/network/CommandSet.cpp                \
        ../common/network/XeNetwork.cpp                 \
        ../common/widgets/RepeatableButton.cpp          \
        ../common/widgets/LCDWithButtons.cpp            \
        ../common/widgets/AutopilotControlWidget.cpp    \
        XeAutopilot.cpp
