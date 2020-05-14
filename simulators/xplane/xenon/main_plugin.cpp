// *********************************************************************************************************************
// *                                          Main file of the X-Plane plugin.                                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 11 mar 2019 at 20:17 *
// *********************************************************************************************************************

// System includes
#include <pthread.h>

#include <string>
#include <thread>
#include <clocale>

// X-Plane Plugin SDK includes
#include "XPLMDefs.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"

// XPMP2 includes
#include "XPMPAircraft.h"
#include "XPMPMultiplayer.h"

// My own includes
#include "xplane_plugin.h"
#include "xplane.hpp"
#include "ivao/whazzup_reader_thread.h"

// Temporary solution for program's settings storage.
#include "settings.h"

#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK and above
#endif

using namespace std;
using namespace xenon;

xenon::XPlanePlugin * _plugin;
pthread_t   _whazzup_reader_thread;
// Был ли корректно создан поток чтения состояния IVAO?
unsigned char _whazzup_gave_birth_to = 0;
const char * plugin_name = "xenon";

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Start of plugin, the plugin's "constructor".                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

PLUGIN_API int XPluginStart( char * outName, char * outSig, char * outDesc ) {

    setlocale(LC_ALL, "C");

    xenon::XPlane::log("Plugin start.");
    // Return plugin's description to X-Plane.

    strcpy(outName, plugin_name);
    strcpy(outSig, "xenon.x-plane.plugin");
    strcpy(outDesc, "An X-Plane plugin for Xenon.");

    // The spawning a plugin variable
    XPLMPluginID this_plugin_id = XPLMGetMyID();
    _plugin = new xenon::XPlanePlugin( this_plugin_id );

#ifdef IVAO    
    int result = pthread_create( &_whazzup_reader_thread, nullptr, & xenon::whazzup_reader_thread, nullptr );
    if ( result == 0 ) {
        // Поток породился корректно.
        _whazzup_gave_birth_to = 0xFF;
    } else {
        // При создании потока возникли ошибки.
        string message = string("IVAO thread create result=") + to_string(result);
        xenon::XPlane::log(message);

    };
#endif


#ifdef DEBUG
    xenon::XPlane::log( "The Xenon plugin starded successfully in ** DEBUG ** mode." );
#endif

    // Чтение и парзинг файлов аэропортов.
    // Происходит в отдельном потоке.
    XPlane::log("Init airports...");
    // auto func = []() {
        Airport::set_has_been_parsed( nullptr );
        Airport::read_all();
        XPlane::log("Init airports done, " + to_string(Airport::count()) + " airports");
    // };
    // std::thread apt_reader( func );
    // apt_reader.detach();

    // The return's value of XPluginStart is logical result, Yes or No.
    return 1;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Stop of plugin, the plugin's "destructor"                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

PLUGIN_API void	XPluginStop(void) {

    xenon::XPlane::log("Plugin stop.");

	// The destroying a plugin variable.

	try {

		if ( _plugin ) {
			delete( _plugin );
			_plugin = nullptr;
		}

	} catch (std::exception & e ) {
		xenon::XPlane::log( std::string("xenon_plugin::XPluginStop(): ") + e.what() );
	} catch ( ... ) {
		xenon::XPlane::log( "xenon_plugin::XPluginStop(): unhandled exception ... :-( ..." );
	}

	// Остановка потока чтения состояния IVAO.
	if (_whazzup_gave_birth_to) {
        pthread_cancel(_whazzup_reader_thread);
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                      Return the observation interval, 1.0 / ( observation frequency )                             *
// *                                                                                                                   *
// *********************************************************************************************************************

inline float get_user_aircraft_observation_interval() {
    auto freq = static_cast<float>( USER_AIRCRAFT_STATE_FREQUENCY );
	return ( (float) 1.0 / freq );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                              Вернуть интервал наблюдения за состоянием окружающей среды                           *
// *                                                                                                                   *
// *********************************************************************************************************************

inline float get_ambient_observation_interval() {
    auto freq = static_cast<float>( AMBIENT_STATE_FREQUENCY );
    return ( (float) 1.0 / freq );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Вернуть интервал наблюдения за состоянием автопилота                               *
// *                                                                                                                   *
// *********************************************************************************************************************

inline float get_autopilot_observation_interval() {
    auto freq = static_cast<float>( AUTOPILOT_STATE_FREQUENCY );
    return ( (float) 1.0 / freq );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Вернуть интервал отслеживания bimbo-самолетов                                    *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
inline float get_bimbo_aircrafts_observation_interval() {
    auto freq = static_cast<float>( BIMBO_AIRCRAFT_FREQUENCY );
    return ( (float) 1.0 / freq );
}
*/

// *********************************************************************************************************************
// *                                                                                                                   *
// *             Callback-функция, периодически вызываемая из XPMP2 для чтения параметров конфигурации.                *
// *                                                                                                                   *
// *********************************************************************************************************************

int cb_xpmp2_configuration(const char * section, const char * item, int default_value) {

#ifdef DEBUG
    // in debug version of the plugin we provide most complete log output
    // if (!strcmp(item, "clamp_all_to_ground")) return 1;  // Прижать самолеты к земле.
    if (!strcmp(item, "model_matching")) return 1;          // Записывать или нет модели в Log.txt
    if (!strcmp(item, "log_level")) return 0;               // DEBUG logging level
#endif
    // Otherwise we just accept defaults
    return default_value;
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                        Callback-процедура наблюдения за состоянием окружающей среды.                             *
// *                                                                                                                  *
// ********************************************************************************************************************

//float cb__observing_ambient(
//        float elapsedSinceLastCall, float elapsedTimeSinceLastFlightLoop, int counter, void * refcon
//) {
//    if ( _plugin && _plugin->is_enabled() ) _plugin->observe_ambient();
//    return get_ambient_observation_interval();
//}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                          Callback-процедура наблюдения за состоянием автопилота.                                 *
// *                                                                                                                  *
// ********************************************************************************************************************

//float cb__observing_autopilot(
//        float elapsedSinceLastCall, float elapsedTimeSinceLastFlightLoop, int counter, void * refcon
//) {
//    if ( _plugin && _plugin->is_enabled() ) _plugin->observe_autopilot();
//    return get_autopilot_observation_interval();
//}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                          Callback-процедура наблюдения и управления bimbo-самолетами                              *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
float cb_observing_bimbo_aircrafts(
        float elapsedSinceLastCall,
        [[maybe_unused]] float elapsedTimeSinceLastFlightLoop,
        [[maybe_unused]] int counter,
        [[maybe_unused]] void * refcon
) {
    if ( _plugin && _plugin->is_enabled() ) {
        _plugin->control_of_bimbo_aircrafts( elapsedSinceLastCall );
    }
    return get_bimbo_aircrafts_observation_interval();
}
*/
// ********************************************************************************************************************
// *                                                                                                                  *
// *                               Callback for observing an aircraft's position and state.                           *
// *                                                                                                                  *
// ********************************************************************************************************************

float cb_observing_user_aircraft(
    [[maybe_unused]] float elapsedSinceLastCall,
    [[maybe_unused]] float elapsedTimeSinceLastFlightLoop,
    [[maybe_unused]] int counter,
    [[maybe_unused]] void * refcon
) {

    if ( _plugin && _plugin->is_enabled() ) _plugin->observe_user_aircraft();
    // The returned value is again observation interval.
    // Accross this time the function will be called once more.
    return get_user_aircraft_observation_interval();

    /*
	// Получение локальной позиции самолета.
    double x = XPLMGetDatad( user_aircraft.acf.x_dataref );
    double y = XPLMGetDatad( user_aircraft.acf.y_dataref );
    double z = XPLMGetDatad( user_aircraft.acf.z_dataref );

    // Перевод позиции самолета в глобальные координаты.
    double lat = 0.0,  lon = 0.0, alt = 0.0;
    XPLMLocalToWorld( x, y, z, & lat, & lon, & alt );

    // Команда для передачи в сеть состояния самолета.
    packet__aircraft_state_t cmd;
    // Пока не знаем, сколько чего нам в эту структуру удастся насобирать.
    // Поэтому "инициализируем" нулями. Вообще-то "на всякий случай", потому что
    // дальше я вроде как отслеживаю возможность не-инициализированных состояний.
    memset( & cmd, 0, sizeof( cmd ) );

    // Заполнение заголовка.
    cmd.header.command = cmdAircraftState;
    cmd.header.length = sizeof( cmd );
    cmd.header.sender = XiSender_X_Plane;

    // Заполнение полей команды.
    cmd.data.latitude = lat;
    cmd.data.longitude = lon;
    cmd.data.altitude = alt;

    cmd.data.heading = XPLMGetDataf( user_aircraft.acf.heading_dataref );

    // Крен и тангаж идут в OpenGL координатах поворота.
    cmd.data.pitch = XPLMGetDataf( user_aircraft.acf.theta_dataref );
    cmd.data.roll = XPLMGetDataf( user_aircraft.acf.phi_dataref );

    // Температура окружающего воздуха.
    cmd.data.ambient_temperature = XPLMGetDataf( user_aircraft.ambient_temp_dataref );

    // cmd.heading = local_pos.heading;
    // cmd.pitch = local_pos.pitch;
    // cmd.roll = local_pos.roll;

    // Инструментальная скорость. Конструкция ? : здесь не работает.
    if ( user_aircraft.ias_dataref ) cmd.data.ias = XPLMGetDataf( user_aircraft.ias_dataref ); else cmd.data.ias = 0.0;
    // Скорость относительно земли.
    if ( user_aircraft.groundspeed_dataref ) cmd.data.groundspeed = XPLMGetDataf( user_aircraft.groundspeed_dataref ); else cmd.data.groundspeed = 0.0;
    // Текущая высота самолета относительно уровня земли.
    if ( user_aircraft.agl_dataref ) cmd.data.agl = XPLMGetDataf( user_aircraft.agl_dataref ); else cmd.data.agl = 0.0;

    // Текущая высота самолета относительно уровня моря, метров.
    // if ( user_aircraft.asl_dataref ) cmd.asl = XPLMGetDatad( user_aircraft.asl_dataref ); else cmd.asl = 0.0;

    // Ускорение в локальных координатах по оси X
    if ( user_aircraft.ax_dataref ) cmd.data.ax = XPLMGetDataf( user_aircraft.ax_dataref ); else cmd.data.ax = 0.0;
    // Ускорение в локальных координатах по оси Y
    if ( user_aircraft.ay_dataref ) cmd.data.ay = XPLMGetDataf( user_aircraft.ay_dataref ); else cmd.data.ay = 0.0;
    // Ускорение в локальных координатах по оси Z
    if ( user_aircraft.az_dataref ) cmd.data.az = XPLMGetDataf( user_aircraft.az_dataref ); else cmd.data.az = 0.0;

    // Угловой момент M
    // if ( user_aircraft.M_dataref ) cmd.M = XPLMGetDataf( user_aircraft.M_dataref ); else cmd.M = 0.0;
    // Угловой момент N
    // if ( user_aircraft.N_dataref ) cmd.N = XPLMGetDataf( user_aircraft.N_dataref ); else cmd.N = 0.0;
    // Угловой момент L
    // if ( user_aircraft.L_dataref ) cmd.L = XPLMGetDataf( user_aircraft.L_dataref ); else cmd.L = 0.0;

    // Скорость вращения по крену.
    if ( user_aircraft.roll_rate_dataref ) cmd.data.roll_rate = XPLMGetDataf( user_aircraft.roll_rate_dataref ); else cmd.data.roll_rate = 0.0;
    // Скорость вращения по тангажу.
    if ( user_aircraft.pitch_rate_dataref ) cmd.data.pitch_rate = XPLMGetDataf( user_aircraft.pitch_rate_dataref ); else cmd.data.pitch_rate = 0.0;
    // Скорость вращения по рысканию.
    if ( user_aircraft.yaw_rate_dataref ) cmd.data.yaw_rate = XPLMGetDataf( user_aircraft.yaw_rate_dataref ); else cmd.data.yaw_rate = 0.0;

    // float arr[8];
    // char out[512];

    // XPLMGetDatavf( user_aircraft.rpm_dataref, &arr[0], 0, 8 );
    // sprintf( out, "arr0=%f", arr[0] );
    // xplugin__log( out );

    // arr[0] = 300;
    // XPLMSetDatavf( user_aircraft.rpm_dataref, &arr[0], 0, 8 );

    // XPLMDataRef d = xplugin__find_dataref("sim/flightmodel/engine/ENGN_prop");

    // XPLMDataRef d = xplugin__find_dataref("sim/flightmodel/engine/POINT_tacrad");
    // XPLMGetDatavf( d, &arr[0], 0, 8 );
    // sprintf( out, "P0=%f", arr[0] );
    // xplugin__log( out );

    // arr[0] = arr[1] = arr[2] = arr[3] = arr[4] = arr[5] = arr[6] = arr[7] = 4000.0;
    // arr[0] = 80.0;
    // XPLMSetDatavf( d, &arr[0], 0, 8 );


    // XPLMDataRef all_rpm = xplugin__find_dataref( "sim/cockpit2/engine/actuators/prop_rotation_speed_rad_sec_all" );
    // XPLMSetDataf( all_rpm, 300.0 );

    // Ускорение вращения по крену.
    // if ( user_aircraft.roll_a_rotation_dataref ) cmd.roll_a_rotation = XPLMGetDataf( user_aircraft.roll_a_rotation_dataref ); else cmd.roll_a_rotation = 0.0;
    // Ускорение вращения по тангажу
    // if ( user_aircraft.pitch_a_rotation_dataref ) cmd.pitch_a_rotation = XPLMGetDataf( user_aircraft.pitch_a_rotation_dataref ); else cmd.pitch_a_rotation = 0.0;
    // Ускорение вращения по рысканию.
    // if ( user_aircraft.yaw_a_rotation_dataref ) cmd.yaw_a_rotation = XPLMGetDataf( user_aircraft.yaw_a_rotation_dataref ); else cmd.yaw_a_rotation = 0.0;

    // Отправка состояния самолета в сеть.
    networking__packet__send ( & cmd, sizeof( cmd ) );
	*/
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Enable of plugin. The plugin can work now.                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

PLUGIN_API int XPluginEnable(void)  {

    // Инициализация XPMP2.
    string separator = XPlane::get_directory_separator();
    string plugin_path = XPlane::get_plugin_path();
    string resource_path = plugin_path + separator + "Resources";

    // Register callback procedures. This made in enable/disable, not in start/stop
    // section, to stop all handling if plugin is disabled.
    XPLMRegisterFlightLoopCallback( cb_observing_user_aircraft, get_user_aircraft_observation_interval(), nullptr );

    // Наблюдение за состоянием автопилота.
    // XPLMRegisterFlightLoopCallback( cb__observing_autopilot, get_autopilot_observation_interval(), NULL );

    // Наблюдение за состоянием окружающей среды.
    // XPLMRegisterFlightLoopCallback( cb__observing_ambient, get_ambient_observation_interval(), NULL );


//    // Наблюдение и установка положений bimbo-самолетов.
//    XPLMRegisterFlightLoopCallback(
//        cb_observing_bimbo_aircrafts, get_bimbo_aircrafts_observation_interval(), nullptr
//    );

    const char * res = XPMPMultiplayerInit (plugin_name,              // plugin name,
                                           resource_path.c_str(),     // path to supplemental files
                                           cb_xpmp2_configuration,   // configuration callback function
                                           "C172");                   // default ICAO type
    if (res[0]) {
        XPlane::log(
                string("Initialization of XPMP2 failed: ") + string(res)
                + ", resource_path=" + resource_path
        );
        return 0;
    }

    // Load our CSL models
    res = XPMPLoadCSLPackage(resource_path.c_str());     // CSL folder root path
    if ( res[0] ) {
        XPlane::log("Error: could not initialize CSL package: " + string(res));
    }

    // Now we also try to get control of AI planes. That's optional, though,
    // other plugins (like LiveTraffic, XSquawkBox, X-IvAp...)
    // could have control already
    res = XPMPMultiplayerEnable();
    if (res[0]) {
        XPlane::log(string("Could not enable AI planes: ") + string(res));
    }

    // Должно быть - в самом конце процедуры, т.к. инициализирует внутреннюю конфигурацию плагина.
    if ( _plugin ) _plugin->enable();
    XPlane::log("Plugin ENABLED.");

    return 1;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             Disable of plugin. The plugin should stop his work now.                               *
// *                                                                                                                   *
// *********************************************************************************************************************

PLUGIN_API void XPluginDisable(void) {

    if ( _plugin ) _plugin->disable();

    // Give up AI plane control
    XPMPMultiplayerDisable();

    // Unregister plane notifier (must match function _and_ refcon)
    // XPMPUnregisterPlaneNotifierFunc(CBPlaneNotifier, NULL);

    // Properly cleanup the XPMP2 library
    XPMPMultiplayerCleanup();

    // Unregister (free) our callback procedures.
    XPLMUnregisterFlightLoopCallback( cb_observing_user_aircraft, nullptr );
    // XPLMUnregisterFlightLoopCallback( cb__observing_autopilot, NULL );
    // XPLMUnregisterFlightLoopCallback( cb__observing_ambient, NULL );
    // XPLMUnregisterFlightLoopCallback( cb_observing_bimbo_aircrafts, nullptr );

    XPlane::log("Plugin DISABLED.");

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Реакция на перезагрузку с интернета файла IVAO whazzup.txt                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void whazzup_reloaded() {
    XPlane::log("Whazzup file was reloaded.");
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                 The plugin received an message from simulator.                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, int messageID, void * ptrParam) {
	if ( _plugin ) _plugin->handle_message( from, messageID, ptrParam );
}

