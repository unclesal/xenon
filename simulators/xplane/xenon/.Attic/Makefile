########################################################################################################################
#                                 Assembling plugin. I refused to using Qt inside of the plugin.                       #
#                                                                                                                      #
# Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                           Created 13 mar 2019 at 11:30 #
########################################################################################################################

# Directory where common (plugin's, client's and server's) files are placed.
COMMON_PATH=../../../common
NETWORK_PATH=$(COMMON_PATH)/network
3DPARTY=../../../3dparty
DISTRIB_FOLDER=../../../distrib

# All objects (.o files) in the project

OBJECTS=build/abstract_aircraft.o			\
	build/abstract_vehicle.o				\
	build/airport.o							\
	build/main_plugin.o						\
	build/windll_main.o 					\
	build/url.o								\
	build/utils.o							\
	build/xplane_plugin.o 					\
	build/XNetworking.o						\
	build/jsonable.o						\
	build/CommandSet.o						\
	build/user_aircraft.o					\
	build/bimbo_aircraft.o					\
	build/Ambient.o							\
	build/XAmbient.o						\
	build/CommandAutopilotState.o			\
	$(3DPARTY)/XPMP2/libXPMP2.a

#	build/base64.o							\
#	build/ivao_periodically_updated_file.o	\
#	build/ivao_status_file.o				\
#	build/ivao_whazzup_file.o				\
#	build/whazzup_reader_thread.o

INCLUDES = -I./ -I../ -I../XPSDK301/CHeaders/XPLM -I$(COMMON_PATH) -I$(NETWORK_PATH)
INCLUDES += -I$(3DPARTY) -I$(3DPARTY)/XPMP2/inc

DEFINES=-DXPLM301 -DXPLM300 -DXPLM200 -DXPLM210 -DINTERNAL_XPLANE -DBOOST_NO_CXX11_SCOPED_ENUMS
FLAGS=-O3 -fPIC -pthread -std=c++2a

DEFINES+=-DDEBUG

# The directory for object files and target file

TEST_BUILD_DIR=@( test -d build || mkdir build );

# xplane_plugin_dir || mkdir ../xplane_plugin_dir; test -d ../xplane_plugin_dir/xenon || mkdir ../xplane_plugin_dir/xenon )

ifeq ($(OS),Windows_NT)
	# The specified definitions for MS Windows
	INSTALL_PATH=/cygdrive/w/program/XPlane11/Resources/plugins/xenon
	TARGET=build/win.xpl
	DEFINES+=-DAPL=0 -DIBM=1 -DLIN=0
	CXX=x86_64-w64-mingw32-g++.exe
	LDFLAGS=-static-libgcc -Wl,-Bstatic -l:libgcc_eh.a -l:libstdc++.a -lws2_32 -l:libwinpthread.a \
		-Wl,-Bdynamic ../XPSDK301/Libraries/Win/XPLM_64.lib -o build/win.xpl
else
	# The specified definitions for Linux
	INSTALL_PATH="/home/sal/program/X-Plane 11/Resources/plugins/xenon"
	TARGET=build/lin.xpl
	DEFINES+=-DAPL=0 -DIBM=0 -DLIN=1
	CXX=g++
	LDFLAGS=-o build/lin.xpl -lpthread
	# -lboost_system -lboost_filesystem
endif

COMPILE = $(CXX) $(DEFINES) $(INCLUDES) $(FLAGS)

# Linking the plugin itself
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -shared -undefined_warning $(LDFLAGS)
	@( chmod a-x $(TARGET) )
	@( test -d $(DISTRIB_FOLDER) || mkdir $(DISTRIB_FOLDER) )
	@( test -d $(DISTRIB_FOLDER)/xplane_plugin_dir || mkdir $(DISTRIB_FOLDER)/xplane_plugin_dir )
	@( test -d $(DISTRIB_FOLDER)/xplane_plugin_dir/xenon || mkdir $(DISTRIB_FOLDER)/xplane_plugin_dir/xenon )
	@( cp -f $(TARGET) $(DISTRIB_FOLDER)/xplane_plugin_dir/xenon/ )

# Installation to X-Plane plugin's directory
install: $(TARGET)
	@( test -d $(INSTALL_PATH) || mkdir $(INSTALL_PATH) )
	cp $(TARGET) $(INSTALL_PATH)

# Uninstall files from X-Plane plugins directory.
# Удаляется - только плагин, не вся папка, т.к. там сидят ресурсы.
uninstall:
	rm -f $(INSTALL_PATH)/lin.xpl

# Clean the project itself

clean:
	$(TEST_BUILD_DIR)
	@( test -d build && rm -rf build )
	@( rm -f $(DISTRIB_FOLDER)/xplane_plugin_dir/xenon/lin.xpl 1>/dev/null 2>/dev/null )
	@( echo "The assembly has been cleaned." )

# Abstract aircraft internal of X-Plane simulator
build/abstract_aircraft.o: abstract_aircraft.cpp abstract_aircraft.h build/abstract_vehicle.o
	$(TEST_BUILD_DIR)
	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c abstract_aircraft.cpp -o build/abstract_aircraft.o

# Любая "самоходка" внутри X-Plane
build/abstract_vehicle.o: abstract_vehicle.cpp abstract_vehicle.h
	$(TEST_BUILD_DIR)
	$(COMPILE) -c abstract_vehicle.cpp -o build/abstract_vehicle.o

# Аэропорт.
build/airport.o: airport.cpp airport.h
	$(TEST_BUILD_DIR)
	$(COMPILE) -c airport.cpp -o build/airport.o

# Самолет - "пустышка" (multiplayer)
build/bimbo_aircraft.o: bimbo_aircraft.cpp bimbo_aircraft.h build/abstract_aircraft.o
	$(TEST_BUILD_DIR)
	$(COMPILE) -c bimbo_aircraft.cpp -o build/bimbo_aircraft.o

# plugin, plugin API interface realization internal of X-Plane simulator
build/main_plugin.o: main_plugin.cpp xplane_utilities.h $(COMMON_PATH)/settings.h build/xplane_plugin.o
	# build/whazzup_reader_thread.o
	$(TEST_BUILD_DIR)
	$(COMPILE) -c main_plugin.cpp -o build/main_plugin.o

# Общие мелкие утилитки
build/utils.o: $(COMMON_PATH)/utils.cpp $(COMMON_PATH)/utils.h
	$(TEST_BUILD_DIR)
	$(COMPILE) -c $(COMMON_PATH)/utils.cpp -o build/utils.o

# Main DLL entry point for MS Windows
build/windll_main.o: windll_main.cpp
	$(TEST_BUILD_DIR)
	$(COMPILE) -c windll_main.cpp -o build/windll_main.o

# C++ Wrapper for X-Plane plugin
build/xplane_plugin.o: xplane_plugin.cpp xplane_plugin.h xplane_utilities.h XSetReactor.h build/XNetworking.o \
	build/bimbo_aircraft.o build/user_aircraft.o build/airport.o
	$(TEST_BUILD_DIR)
	$(COMPILE) -c xplane_plugin.cpp -o build/xplane_plugin.o

# class which can be present in JSON form.
build/jsonable.o: $(COMMON_PATH)/jsonable.cpp $(COMMON_PATH)/jsonable.h
	$(TEST_BUILD_DIR)
	$(COMPILE) -c $(COMMON_PATH)/jsonable.cpp -o build/jsonable.o

# Команда установки - чего-нибудь.
build/CommandSet.o: build/jsonable.o $(NETWORK_PATH)/CommandSet.cpp $(NETWORK_PATH)/CommandSet.h
	$(TEST_BUILD_DIR)
	$(COMPILE) -c $(NETWORK_PATH)/CommandSet.cpp -o build/CommandSet.o

# Состояние автопилота.
build/CommandAutopilotState.o: $(NETWORK_PATH)/CommandAutopilotState.cpp $(NETWORK_PATH)/CommandAutopilotState.h build/jsonable.o
	$(TEST_BUILD_DIR)
	$(COMPILE) -c $(NETWORK_PATH)/CommandAutopilotState.cpp -o build/CommandAutopilotState.o

# Сеть внутри плагина. Зависит от сетевых команд и их представления внутри симулятора X-Plane
build/XNetworking.o: XNetworking.cpp XNetworking.h $(COMMON_PATH)/settings.h xplane_utilities.h build/jsonable.o \
	XSetReactor.h build/abstract_aircraft.o build/user_aircraft.o build/Ambient.o \
	build/XAmbient.o build/CommandAutopilotState.o build/CommandSet.o
	$(TEST_BUILD_DIR)
	$(COMPILE) -c XNetworking.cpp -o build/XNetworking.o

# Окружающая действительность.
build/Ambient.o: build/jsonable.o $(NETWORK_PATH)/Ambient.cpp $(NETWORK_PATH)/Ambient.h
	$(TEST_BUILD_DIR)
	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c $(NETWORK_PATH)/Ambient.cpp -o build/Ambient.o

build/XAmbient.o: build/Ambient.o XAmbient.cpp XAmbient.h
	$(TEST_BUILD_DIR)
	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c XAmbient.cpp -o build/XAmbient.o

build/url.o: $(NETWORK_PATH)/url.cpp $(NETWORK_PATH)/url.h
	$(TEST_BUILD_DIR)
	$(COMPILE) -c $(NETWORK_PATH)/url.cpp -o build/url.o

# User aicraft internal of X-Plane simulator, which always have index 0.
build/user_aircraft.o: user_aircraft.cpp user_aircraft.h xplane_utilities.h build/abstract_aircraft.o
	$(TEST_BUILD_DIR)
	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c user_aircraft.cpp -o build/user_aircraft.o

########################################################################################################################
#                                                                                                                      #
#                                                        IVAO                                                          #
#                                                                                                                      #
########################################################################################################################
#
## base64 кодер/декодер
#build/base64.o: $(3DPARTY)/base64.cpp $(3DPARTY)/base64.h
#	$(TEST_BUILD_DIR)
#	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c $(3DPARTY)/base64.cpp -o build/base64.o
#
## Периодически обновляемый IVAO-файл.
#build/ivao_periodically_updated_file.o: ivao/ivao_periodically_updated_file.cpp ivao/ivao_periodically_updated_file.h build/base64.o
#	$(TEST_BUILD_DIR)
#	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c ivao/ivao_periodically_updated_file.cpp -o build/ivao_periodically_updated_file.o
#
## Файл состояния IVAO.
#build/ivao_status_file.o: build/ivao_periodically_updated_file.o ivao/ivao_status_file.h ivao/ivao_status_file.cpp xplane_utilities.h
#	$(TEST_BUILD_DIR)
#	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c ivao/ivao_status_file.cpp -o build/ivao_status_file.o
#
#build/ivao_whazzup_file.o: ivao/ivao_whazzup_file.cpp ivao/ivao_whazzup_file.h build/ivao_status_file.o build/ivao_periodically_updated_file.o build/url.o
#	$(TEST_BUILD_DIR)
#	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c ivao/ivao_whazzup_file.cpp -o build/ivao_whazzup_file.o
#
## Поток чтения состояния IVAO
#build/whazzup_reader_thread.o: ivao/whazzup_reader_thread.cpp ivao/whazzup_reader_thread.h xplane_utilities.h \
#	$(COMMON_PATH)/settings.h build/ivao_status_file.o build/ivao_whazzup_file.o
#
#	$(TEST_BUILD_DIR)
#	$(CXX) $(DEFINES) $(INCLUDES) $(FLAGS) -c ivao/whazzup_reader_thread.cpp -o build/whazzup_reader_thread.o
