// *********************************************************************************************************************
// *                                  The transition from XPlane API to C ++ /Qt basis.                                *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 mar 2019 at 15:36 *
// *********************************************************************************************************************

// system includes.

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netdb.h>

// XPlane Plugin SDK includes
#include "XPLMPlugin.h"
#include "XPLMPlanes.h"

// 3rd party includes

// My own includes.
#include "xplane_plugin.h"
#include "xplane.hpp"

using namespace xenon;
using namespace std;

char __receiv_buffer[ COMMUNICATOR_MAX_PACKET_SIZE ];

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  The constructor                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

XPlanePlugin::XPlanePlugin( XPLMPluginID & this_plugin_id ) {

    Airport::read_all();
    
    // Remember ID of our plugin for the future communications between plugins.
    __this_plugin_id = this_plugin_id;
    __enabled = false;
    __inited = false;
    __socket = -1;
    
    // __networking = XNetworking::create();
    // __networking->set_setter( this );

    __uair_count = 0;   
    __connected = false;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Enable plugin's work.                                              *
// *   It is called, except other cases, from the X-Plane plugin manager,  when enabling checkbox was engaged off      *
// *                                              and then engagged on back.                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::enable() {
    __enabled = true;    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Disable plugin's work.                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::disable() {
    __enabled = false;        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Один "шаг" работы с сетью                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::network_tick() {
    if ( ! __inited ) return;
    if ( ! __connected ) __try_open_socket();
    if ( __connected ) __read_from_socket();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Попытка открыть сокету и соединиться с коммуникатором                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::__try_open_socket() {
    
    if ( ( __socket < 0 ) || ( ! __connected ) ) {            
        
        // Попытка открытия сокеты. Причем, может быть такая ситуация, что сама сокета-то 
        // открыта, но вот соединиться мы не смогли. Учитываем эту ситуацию.        
        __connected = false;
        if ( __socket >= 0 ) disconnect();
        
        __socket = socket(AF_INET, SOCK_STREAM, 0);        
        if ( __socket < 0 ) return;
        
        hostent * remote_communicator = gethostbyname( get_communicator_host().c_str() );
        if ( ! remote_communicator ) {
            disconnect();
            on_error(
                "ConnectedCommunictor::__receiv(), no such host " + get_communicator_host()
            );            
            return;
        }
        
        sockaddr_in server_addr;
        
        memset( &server_addr, 0, sizeof( server_addr ) );
        server_addr.sin_family = AF_INET;
        bcopy((char *) remote_communicator->h_addr, (char *) & server_addr.sin_addr.s_addr, remote_communicator->h_length);            
        server_addr.sin_port = htons( get_communicator_port() );
        
        if ( ::connect( __socket, (struct sockaddr * ) & server_addr, sizeof(server_addr)) < 0) {
            if ( errno != ECONNREFUSED ) // connection refused
            {
                on_error(
                    "ConnectedCommunicator::connect: " + std::to_string( errno ) 
                    + ", message=" + std::string( strerror( errno ))
                );
            }
            disconnect();
            return;
        }

        // Если здесь остались - мы соединены.
        // Делаем сокету не-блоковой.
        
        int flags = fcntl( __socket, F_GETFL, 0);
        flags |= O_NONBLOCK;
        fcntl( __socket, F_SETFL, flags);
        
        // Устанавливаем факт соединенности и вызываем обработчик соединения в плагине.
        __connected = true;
        on_connect();

    };
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Чтение информации с сокеты.                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::__read_from_socket() {
    for ( ;; ) {
        uint16_t len = 0;
        ssize_t bytes = recv( __socket, & len, sizeof( len ), MSG_PEEK );
        if ( bytes < 0 ) {
            // Сетевая ошибка.
            disconnect();
            return;
        }
        
        if ( bytes < sizeof( len ) ) return; // Там нет даже длины.
        
        int total;        
        ioctl( __socket, FIONREAD, & total );
        if ( total >= len + sizeof( len ) ) {
            
            // Там есть полная команда. Вычитываем ее полностью.
            bytes = read( __socket, & len, sizeof( len ) );
            bytes = read( __socket, __receiv_buffer, len );
            
            if ( bytes < 0 ) {
                disconnect();
                return;
            };
            
            if ( bytes != len ) {
                on_error("Read from socket, want " + std::to_string( len ) + ", but read " + std::to_string( bytes) );
                return;
            }
            // В буфере получили полностью пакет с командой.
            
            XPlane::log("Readed: " + std::to_string( bytes ) );
            
        } else return;
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Рассоединиться с сетью                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::disconnect() {
    
    if ( __socket >= 0 ) {
        ::close( __socket );
        __socket = -1;
    }
    __connected = false;
    
    on_disconnect();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       User's aircraft was loaded into simulator.                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::__user_aircraft_was_loaded() {
    XPlane::log("User aircraft was (re-)loaded");
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Observe user aircraft position and state.                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::observe_user_aircraft() {

    if ( __enabled ) {
        __user_aircraft.observe();
        
        // __networking->send_to_all( & __user_aircraft );

        // "Отложенная" инициализация.

        if ( ! __inited ) {
            
            __uair_count ++;
            // Начиная с некоторого тика - ждем, когда будет дочитаны
            // полностью все имеющиеся аэропорты.
            if ( __uair_count >= 4 ) {
                __init_around();
            }
        }

    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Инициализация  окружающей среды                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::__init_around() {

    if ( __inited ) return;
    
    // Если аэропорт еще не доинициализировался, то пытаться пока рановато еще.
    if ( ! Airport::airports_was_readed() ) return;
    
    if ( ! __connected ) __try_open_socket();
    __inited = true;

    
//     auto acf1 = new BimboAircraft("B763", "SAS", "SAS");
//     auto usss = Airport::get_by_icao("USSS");
//     auto gate = usss.get_startup_locations()["10"];
//     acf1->place_on_ground( gate );    
//     __bimbos.push_back( acf1 );
    
//     auto rotation = acf1->get_rotation();
//     rotation.pitch = acf1->parameters().take_off_angle;
//     acf1->set_rotation( rotation );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Соединились с коммуникатором                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::on_connect() {
    
    __user_aircraft.update_conditions();
    
    AbstractCommandTransmitter trx( __socket );
    std::string error;
        
    // Заявляем о себе.
    CmdAircraftCondition current_condion( __user_aircraft.vcl_condition, __user_aircraft.acf_condition );
    if ( ! trx.transmitt( & current_condion, error ) ) on_error( error );
    
    // Спрашиваем об окружающих.
    error = "";
    CmdQueryAround query_arround( __user_aircraft.vcl_condition );
    if ( ! trx.transmitt( & query_arround, error )) on_error( error );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Произошло отсоединение от коммуникатора                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::on_disconnect() {
    XPlane::log("Communicator - disconnected...");
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     С коммуникатора был получен пакет команды                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::on_received( void * abstract_command ) {
    
    if ( ! abstract_command ) return;
    AbstractCommand * cmd = ( AbstractCommand * ) abstract_command;
    
    Logger::log( "Got " + cmd->command_name() );
    
    /*
    
    // Порядок - имеет значение!!! Наследование потому что.
    CmdAircraftCondition * cmd_aircraft_condition = dynamic_cast< CmdAircraftCondition * >( cmd );
    if ( cmd_aircraft_condition ) {
        __command_received( cmd_aircraft_condition );
        return;
    };
    
    CmdFlightPlan * cmd_flight_plan = dynamic_cast< CmdFlightPlan * > ( cmd );
    if ( cmd_flight_plan ) {
        __command_received( cmd_flight_plan );
        return;
    };
    
    CmdWaypointReached * cmd_waypoint_reached = dynamic_cast < CmdWaypointReached * > ( cmd );
    if ( cmd_waypoint_reached ) {
        __command_received( cmd_waypoint_reached );
        return;
    };
    
    XPlane::log("XPlanePlugin::on_received: " + cmd->command_name() + " received, but unhandled" );
    */
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   При работе с коммуникатором произошли ошибки                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::on_error( std::string message ) {
    XPlane::log("ERROR with communications: " + message );
    disconnect();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Наблюдение за состоянием окружающей среды.                                     *
// *                                                                                                                   *
// *********************************************************************************************************************
//
//void xenon::XPlanePlugin::observe_ambient() {
//
//    if ( __enabled ) {
//        __ambient.observe();
//        __networking->send_to_all( & __ambient );
//    }
//
//}
//
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Наблюдение за состоянием автопилота                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

//void xenon::XPlanePlugin::observe_autopilot() {
//
//    if ( __enabled ) {
//        __autopilot.observe();
//        __networking->send_to_all( & __autopilot );
//    }
//
//}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Наблюдение и управление bimbo-самолетами                                       *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void xenon::XPlanePlugin::control_of_bimbo_aircrafts( float elapsed_since_last_call ) {
    // Цикл по всем имеющимся "управляемым самолетам".
    for ( auto it : __bimbos ) {
        it->control( elapsed_since_last_call );
    }
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      The AI aircraft was loaded into simulator.                                   *
// *                                         Parameter is index of loaded aircraft.                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::__ai_controlled_aircraft_was_loaded( int index ) {
    XPlane::log( std::string("Controlled aircraft was loaded, index=") + std::to_string(index) );

//    AIControlledAircraft acf(index);
//    __ai_controlled_aircrafts[index] = acf;
//    if ( index == 1 ) {
//        AbstractObservedObject::position_t pos = __user_aircraft.get_position();
//        // pos.x += 0.1;
//        // pos.y += 0.1;
//        __ai_controlled_aircrafts[1].set_position(pos);
//    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Установка - чего-нибудь - извне.                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

/*
void XPlanePlugin::set( CommandSet & cmd ) {

    if ( ! __enabled ) return;

    if ( cmd.section() == CommandSet::SECTION_AUTOPILOT ) {

//        // Установка переменных автопилота.
//        __autopilot.set(cmd);
//        // После установки  - читаем новые значения и передаем их обратно.
//        __autopilot.observe();
//        __networking->send_to_all( & __autopilot );

    } else if ( cmd.section() == CommandSet::SECTION_USER_AIRCRAFT ) {

        // Пользовательский самолет.
//        __user_aircraft.set( cmd );
//        __user_aircraft.observe();
//        __networking->send_to_all( & __user_aircraft );

    } else {
        char out[256];
        sprintf(out, "ERROR: XPlanePlugin::set(), unhandled section %s", cmd.section().c_str() );
        XPlane::log( out );

    }
}
*/

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Handling a messages from the simulator.                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::handle_message(XPLMPluginID from, int messageID, void * ptrParam) {

    // Many procedures have a parameter. It is a number, not a pointer.
    // This is uncomfortable to convert it every each time.
    // Therefore i make a conversion here.

    size_t parameter = ( size_t ) ptrParam;
    int iparam = static_cast<int>( parameter );

    switch ( from ) {

        case XPLM_PLUGIN_XPLANE : {

            // ------------------------------------------------------------------------------------
            //                    The message was received from the X-Plane itself.
            // ------------------------------------------------------------------------------------

            switch ( messageID ) {

                case XPLM_MSG_PLANE_LOADED : {

                    // An aircraft was loaded. The parameter is a index of loaded airplane.

                    if ( parameter ==  XPLM_USER_AIRCRAFT ) {

                        // The user's aircraft was loaded.
                        __user_aircraft_was_loaded();
                        return;

                    } else {

                        // The aircraft was loaded but it is not user's aircraft.
                        // Create this aircraft if we still have'nt it.
                        __ai_controlled_aircraft_was_loaded( parameter );
                        return;

                    };

                }; break;

                case XPLM_MSG_AIRPLANE_COUNT_CHANGED : {
                    // The amount of available aircrafts has been changed in the X-Plane.
                    char out[256];
                    sprintf( out, "Airplane count changed to %d", iparam );
                    XPlane::log( out );
                    return;

                }; break;

                case XPLM_MSG_PLANE_CRASHED : {
                    // An airplane was crashed.
                    // msg__airplane_crashed( parameter );
                    return;
                }; break;

                case XPLM_MSG_PLANE_UNLOADED : {
                    // Самолет был выгружен внутри X-Plane
                    if ( iparam > 0 ) {
                        // Это был самолет AI-traffic. Нужно убрать данный
                        // индекс из коллекции управляемых нами самолетов.
                    }
                    char out[256];
                    sprintf( out, "Aircraft was unloaded, param=%d", iparam );
                    XPlane::log( out );
                    return;
                }; break;

                case XPLM_MSG_AIRPORT_LOADED : {

                    // An airport was loaded. This message does not appear in the time of game,
                    // it sends only when user change an aicraft or an airport by hands.
                    XPlane::log("Airport was loaded");
                    return;

                }; break;

                case XPLM_MSG_SCENERY_LOADED : {
                    // A scenery was loaded.
                    XPlane::log("Scenery was loaded");
                    return;

                }; break;

                case XPLM_MSG_LIVERY_LOADED : {

                    // msg__livery_loaded( parameter );
                    return;

                }; break;

                case XPLM_MSG_WILL_WRITE_PREFS : {
                    // X-Plane gonna to write himself settings and ask us to participate.
                    // msg__will_write_prefs();
                    return;
                }; break;

                default: {
#ifdef DEBUG
                    XPlane::log(
                        std::string("XPlanePlugin::handleMessage(), unhandled message type from X-Plane ")
                        + std::to_string( messageID )
                    );
#endif
                };

            }; // switch inMessage (only from x-plane)

        }; break; // case messaage from xplane

        default : {

#ifdef DEBUG
            XPlane::log(
                std::string("A message from another plugin id=")
                + std::to_string(from) + " has been received but unhandled. MessageID="
                + std::to_string(messageID) + ", parameter=" + std::to_string( parameter )
            );
#endif
        };

    }; // end of switch inFromWho

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                  Добавление внешнего самолета ("агента") в коллекцию, если его там еще не было                    *
// *                                                                                                                   *
// *********************************************************************************************************************

BimboAircraft * XPlanePlugin::__add_one_aircraft( CmdAircraftCondition * cmd ) {
    
    Logger::log("Create new one");
    auto acf_condition = cmd->acf_condition();    
    BimboAircraft * bimbo = new BimboAircraft(
        acf_condition.icao_type, acf_condition.icao_airline, acf_condition.livery
    );        
    
    bimbo->label = cmd->vcl_condition().agent_name;
    if ( cmd->vcl_condition().agent_type == AGENT_AIRCRAFT ) {
        // Данный самолет является отражением внешнего агента.            
        bimbo->colLabel[0] = 0.0f;  // R
        bimbo->colLabel[1] = 1.0f;  // G
        bimbo->colLabel[2] = 0.0f;  // B

    } else if ( cmd->vcl_condition().agent_type == AGENT_XPLANE ) {
        // Данный самолет - это человек, зашедший по сети в X-Plane.
        bimbo->colLabel[0] = 0.0f;  // R
        bimbo->colLabel[1] = 0.0f;  // G
        bimbo->colLabel[2] = 1.0f;  // B

    } else {
        XPlane::log("BimboAircraft::update_from(), unhandled agent type " + to_string( cmd->vcl_condition().agent_type ));
    }        
    
    // Если самолет только что был добавлен, то тут не надо "плавностей". Он же 
    // только что появился на экране. Должен быть сразу же - таким, какой он есть.
    
    if ( acf_condition.is_gear_down ) bimbo->v[ XPMP2::V_CONTROLS_GEAR_RATIO ] = 1.0;
    bimbo->v[ XPMP2::V_CONTROLS_FLAP_RATIO ] = acf_condition.flaps_position;
    bimbo->v[ XPMP2::V_CONTROLS_SPEED_BRAKE_RATIO ] = acf_condition.speed_brake_position;
    bimbo->v[ XPMP2::V_CONTROLS_THRUST_RATIO ] = acf_condition.thrust_position;
    
    __agents_mutex.lock();    
    __bimbos.push_back( bimbo );
    __agents_mutex.unlock();
    
    return bimbo;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                 Получена команда о состоянии внешнего самолета                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::__command_received( CmdAircraftCondition * cmd ) {            
            
    BimboAircraft * bimbo = nullptr;
    bool was_appended = false;
    
    for ( auto b : __bimbos ) {
        if ( b->agent_uuid() == cmd->agent_uuid() ) {
            bimbo = b;
            break;
        };
    };
        
    if ( ! bimbo ) {
        // Самолетика нет, он только что создается.
        bimbo = __add_one_aircraft( cmd );                        
    };
        
    bimbo->update_from( cmd->vcl_condition(), cmd->acf_condition() );        
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Получен полетный план внешнего самолета                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::__command_received( CmdFlightPlan * cmd ) {
        
    __agents_mutex.lock();
    bool found = false;
    for ( auto bimbo: __bimbos ) {
        if ( bimbo->agent_uuid() == cmd->agent_uuid() ) {
            found = true;
            // Коррекции высот не происходит, ее сделает действие посадки.
            bimbo->flight_plan = cmd->flight_plan();            
            break;
        };
    };
    __agents_mutex.unlock();
    if ( ! found ) {
        Logger::log("CmdFlightPlan received, agent " + cmd->agent_uuid() + " not found in collection");
    };
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                              В основном агенте была достигнута точка полетного плана                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void XPlanePlugin::__command_received( CmdWaypointReached * cmd ) {
    
    __agents_mutex.lock();
    for ( auto bimbo: __bimbos ) {
        if ( bimbo->agent_uuid() == cmd->agent_uuid() ) {
            bimbo->flight_plan.erase_up_to( cmd->npp() );
            break;
        };
    };
    __agents_mutex.unlock();
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Деструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

XPlanePlugin::~XPlanePlugin() {

    disconnect();
    
    __agents_mutex.lock();
    // Корректное освобождение памяти, выделенной под самоходки.
    for ( auto vcl : __vehicles ) {
        delete( vcl );
    }
    __vehicles.clear();
    
    // Корректное освобождение памяти под самолеты.
    for ( auto bimbo : __bimbos ) {
        delete bimbo;
    }
    __bimbos.clear();
    
    __agents_mutex.unlock();
    
}
