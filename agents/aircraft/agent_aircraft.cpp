// *********************************************************************************************************************
// *                                              Реализация агента самолета                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:49 *
// *********************************************************************************************************************
#include <stdlib.h>

#include "agent_aircraft.h"
#include "tested_agents.h"
#include "cmd_query_around.h"
#include "cmd_aircraft_condition.h"
#include "cmd_flight_plan.h"
#include "cmd_waypoint_reached.h"

#include "push_back_allowed.h"
#include "taxing_distance.h"
#include "hp_lu_occupated.h"
#include "hp_some_one_landing.h"
#include "lu_before_take_off.h"
#include "taxing_push_back_ahead.h"
#include "aircraft_state_landed.h"

using namespace xenon;
using namespace std;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AgentAircraft::AgentAircraft ( const std::string & uuid ) : AbstractAgent() {
    
    __cycles = 0;
    
    __started = false;
    __start_time = get_system_time_ms();
    
    if ( ! Airport::airports_was_readed() ) {
        Airport::read_all();
    }

    // Ждем, пока аэропорт не вычитает свои данные.
    while ( ! Airport::airports_was_readed() ) {
        usleep(500);
    }
    
    __temporary_make_aircraft_by_uuid( uuid );
            
    if ( ! __ptr_acf ) {
        Logger::log("AgentAircraft::AgentAircraft() UUID " + uuid + " not handled");
        return;
    };
    
    __init_parking_frames();
    __init_taxing_frames();
    __init_hp_frames();
    __init_lu_frames();
    
    // Коммуникатор порождаем последним, т.к. там потоки и он может тут же 
    // соединиться. Нужно, чтобы все указатели были уже инициализированы.
    _create_communicator();
    
    __ptr_acf->set_agent( this ); 

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                      Инициализация фреймов, имеющих отношение к состоянию "на парковке"                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__init_parking_frames() {
    
    PushBackAllowed * pb = new PushBackAllowed( __ptr_acf, this );
    __state_frames[ ACF_STATE_PARKING ].push_back( pb );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Инициализация фреймов, имеющих отношение к состоянию руления                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__init_taxing_frames() {
    
    TaxingDistance * dis = new TaxingDistance( __ptr_acf, this );
    __state_frames[ ACF_STATE_READY_FOR_TAXING ].push_back( dis );
    
    TaxingPushBackAhead * taxing_push_back_ahead = new TaxingPushBackAhead( __ptr_acf, this );
    __state_frames[ ACF_STATE_READY_FOR_TAXING ].push_back( taxing_push_back_ahead );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Инициализация фреймов, имеющих отношение к предварительному старту                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__init_hp_frames() {
    
    HpLuOccupated * hp_lu_occupated = new HpLuOccupated( __ptr_acf, this );
    __state_frames[ ACF_STATE_HP ].push_back( hp_lu_occupated );
    
    HpSomeOneLanding * hp_some_one_landing = new HpSomeOneLanding( __ptr_acf, this );
    __state_frames[ ACF_STATE_HP ].push_back( hp_some_one_landing );
    
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *         Инициализация фреймов, имеющих отношение к состоянию "готов к взлету" (исполнительный старт)              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__init_lu_frames() {
    
    LUBeforeTakeOff * lub = new LUBeforeTakeOff( __ptr_acf, this );
    __state_frames[ ACF_STATE_READY_FOR_TAKE_OFF ].push_back( lub );
    
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Времянка: создание самолета по его UUIDу                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__temporary_make_aircraft_by_uuid( const std::string & uuid ) {
    
    auto usss = Airport::get_by_icao("USSS");
    auto gate = usss.get_startup_locations()["15"];
    
    if ( uuid == B738_AFF ) {        
        
        __ptr_acf = new BimboAircraft("B738", "AFF", "AFF");
        __ptr_acf->vcl_condition.agent_name = "Boeing 737-800 AFF";
        gate = usss.get_startup_locations()["15"];        
        __start_time += 1000;
        
    } else if ( uuid == B738_SBI ) {
        
        __ptr_acf = new BimboAircraft( "B738", "SBI", "SBI" );
        __ptr_acf->vcl_condition.agent_name = "Boeing 737-800 SBI";
        gate = usss.get_startup_locations()["14"];
        __start_time += 2000;
        
    } else if ( uuid == A321_AFL ) {
        
        __ptr_acf = new BimboAircraft("A321", "AFL", "AFL");
        __ptr_acf->vcl_condition.agent_name = "Airbus A321 AFL";                
        gate = usss.get_startup_locations()["13"];
        __start_time += 3000;
        
        
    } else if ( uuid == A321_SVR ) { 
        
        // Airbus A321 Уральские Авиалинии.
        
        __ptr_acf = new BimboAircraft("A321", "SVR", "SVR");
        __ptr_acf->vcl_condition.agent_name = "Airbus A321 SVR";                
        gate = usss.get_startup_locations()["12"];        
        __start_time += 4000;
        
    } else if ( uuid == B772_UAE ) {
        
        // Boeing 777-200 UAE
        
        __ptr_acf = new BimboAircraft("B772", "UAE", "UAE");
        __ptr_acf->vcl_condition.agent_name = "Boeing 777-200 UAE";        
        gate = usss.get_startup_locations()["10"];        
        __start_time += 5000;
        
    } else if ( uuid == B744_SWI ) {
        
        __ptr_acf = new BimboAircraft( "B744", "SWI", "SWI" );
        __ptr_acf->vcl_condition.agent_name = "Boeing 747-400 SWI";
        gate = usss.get_startup_locations()["9"];
        __start_time += 6000;
        
    } else if ( uuid == B763_SAS ) {
        
        // Boeing 767-300 El-Al
        __ptr_acf = new BimboAircraft( "B763", "ELY", "ELY" );
        __ptr_acf->vcl_condition.agent_name = "Boeing 767-300 SAS";
        gate = usss.get_startup_locations()["8"];
        __start_time += 7000;
        
    } else if ( uuid == B744_THA ) {
        
        // Boeing 747-400 THA
        
        __ptr_acf = new BimboAircraft( "B744", "THA", "THA" );
        __ptr_acf->vcl_condition.agent_name = "Boeing 747-400 THA";
        gate = usss.get_startup_locations()["7"];
        __start_time += 8000;
    }
    
    if ( __ptr_acf ) {
        __ptr_acf->vcl_condition.agent_uuid = uuid;
        __ptr_acf->vcl_condition.agent_type = AGENT_AIRCRAFT; 
        
        // __test_landing();
        __test_fly_circle( usss, gate );
        
//         Коррекция высот ВПП.
//         for ( int i=0; i<__ptr_acf->flight_plan.size(); i++ ) {
//             auto wp = __ptr_acf->flight_plan.get( i );
//             if ( wp.type == WAYPOINT_RUNWAY || wp.type == WAYPOINT_DESTINATION ) {
//                 wp.location.altitude = usss.evalution_in_meters();
//                 __ptr_acf->flight_plan.set( i, wp );
//             };
//         };
                                                               
    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Тестирование - полета по кругу, "коробочки"                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__test_fly_circle( Airport & airport, const startup_location_t & gate ) {
    
    __ptr_acf->place_on_ground( gate );
         
    auto where_i_am = __ptr_acf->get_location();    
    auto way = airport.get_taxi_way_for_departure( where_i_am );
    __ptr_acf->prepare_for_take_off( way );
    __ptr_acf->test__fly();    

}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Тестирование - только посадочной фазы                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__test_landing() {
    
    __ptr_acf->test__fp_landing();
    waypoint_t first = __ptr_acf->flight_plan.get(0);
    __ptr_acf->vcl_condition.is_clamped_to_ground = false;
    __ptr_acf->set_location( first.location );
    
    rotation_t rotation;
    rotation.heading = 80;
    rotation.pitch = 0;
    rotation.roll = 0;
    
    __ptr_acf->set_rotation( rotation );
    
    __ptr_acf->graph->set_active_state( ACF_STATE_ON_THE_FLY );
    auto action = __ptr_acf->graph->get_action_outgoing_from_current_state( ACF_DOES_FLYING );
    __ptr_acf->graph->set_active_action( action );
    
    __ptr_acf->vcl_condition.speed = __ptr_acf->parameters().cruise_speed;
    __ptr_acf->vcl_condition.target_speed = __ptr_acf->vcl_condition.speed;
    __ptr_acf->vcl_condition.acceleration = 0.0;
    
    __ptr_acf->flight_plan.set_departure("USSS");
    __ptr_acf->flight_plan.set_destination("USSS");    

}



// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Метод выполнения агента самолета                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::run() {
    
    if ( ! __ptr_acf ) {
        Logger::log("AgentAircraft::run(): acf pointer is none. Exit.");
        return;
    }

    __previous_time = xenon::get_system_time_ms();
    __cycles = 0;        
    
    for (;;) {
        // Время - в микросекундах.
        usleep( AGENT_TICK );
        
        if ( ! __started ) {
            long int current_time = get_system_time_ms();
            if ( current_time >= __start_time ) {
                __started = true;
                cout << __ptr_acf->vcl_condition.agent_name << " started" << endl;                
            }
        }        
        
        __step();
        __cycles ++;
        
        if ( __cycles >= CYCLES_PER_SCREAM ) {
            __cycles = 0;
            scream_about_me();            
        }
        
        if ( __started ) __decision();

    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Выбор следующего действия                                              *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void AgentAircraft::__choose_next_action() {  
    
    string current_state_name = "Unknown state";
    auto current_state = __ptr_acf->graph->get_current_state();
    if ( current_state ) {
        auto node = __ptr_acf->graph->get_node_for( current_state );
        current_state_name = node.name;
    }
    
    Logger::log("choose_next_action(), state=" + current_state_name + ", fp size=" + to_string( __ptr_acf->_flight_plan.size() ) );
    
    if (
        ( __ptr_acf->graph->current_state_is( ACF_STATE_PARKING )) 
        && ( __ptr_acf->graph->current_action_is( ACF_DOES_NOTHING ))
        && ( __ptr_acf->__taxing_prepared )
        && ( ! __ptr_acf->_flight_plan.empty() )
    ) {
        // На парковке и "ничего не делает", и при этом рулежка уже
        // подготовлена - поехали по полетному плану.
        __ptr_acf->__start_fp0_action();
        return;
    }
        
    if (
        ( __ptr_acf->graph->current_state_is( ACF_STATE_READY_FOR_TAXING ) )
        && ( __ptr_acf->__taxing_prepared )
    ) {
        // Если готов к рулению - пока что поехали. 
        // TODO: всякая фигня типа заведения двигателей, разрешения на руление и др.
        __ptr_acf->__start_fp0_action();
        return;
    } 
    
    if ( __ptr_acf->graph->current_state_is( ACF_STATE_HP ) ) {
        __ptr_acf->__start_fp0_action();
        return;
    }
    
    if ( __ptr_acf->graph->current_state_is( ACF_STATE_READY_FOR_TAKE_OFF ) ) {
        __ptr_acf->__start_fp0_action();
        return;
    };
    
    if ( __ptr_acf->graph->current_state_is( ACF_STATE_AIRBORNED ) ) {
        
        aircraft_state_graph::graph_t::edge_descriptor action 
            = __ptr_acf->graph->get_action_outgoing_from_current_state( ACF_DOES_FLYING );
            
        __ptr_acf->graph->set_active_action( action );
        
        return;
    };
    
    if ( __ptr_acf->graph->current_state_is ( ACF_STATE_ON_FINAL ) ) {
        __ptr_acf->__start_fp0_action();
        return;
    }

    if ( __ptr_acf->graph->current_state_is( ACF_STATE_LANDED )) {
        if ( !__ptr_acf->_flight_plan.empty() ) __ptr_acf->__start_fp0_action();
        else Logger::log("AgentAircraft::__choose_next_action(), ACF_STATE_LANDED, but FP is empty");
        return;
    }
    
    if ( __ptr_acf->graph->current_state_is( ACF_STATE_RUNWAY_LEAVED )) {
        __ptr_acf->__start_fp0_action();
        return;
    };
    
    if ( __ptr_acf->graph->current_state_is( ACF_STATE_BEFORE_PARKING )) {
        __ptr_acf->__start_fp0_action();
        return;
    };
    
    Logger::log("ERROR: AgentAircraft::__choose_next_action(), action was not determined");    
};
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                     Отправить в сеть пакет своего состояния, если коммуникатор соединен                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::scream_about_me() {
    if ( ! __ptr_acf ) return;
    if ( ! _communicator ) return;
    if ( _communicator->is_connected()  ) {
        CmdAircraftCondition * cmd = new CmdAircraftCondition( __ptr_acf->vcl_condition, __ptr_acf->acf_condition );
        _communicator->request(cmd);
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    В графе самолета изменилось состояние                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::state_changed( void * state ) {
    
    scream_about_me();
    
    // После посадки нужно определить стоянку.
    AircraftAbstractState * abstract_state = ( AircraftAbstractState * ) state;
    
    cout << __ptr_acf->vcl_condition.agent_name << ", AgentAircraft::state_changed to " << __ptr_acf->graph->get_node_for( abstract_state ).name << endl;
    
    AircraftStateLanded * landed = dynamic_cast< AircraftStateLanded * >( abstract_state );
    if ( landed ) {
                
        waypoint_t wp = __ptr_acf->flight_plan.get(0);
        while ( wp.type == WAYPOINT_RUNWAY || wp.type == WAYPOINT_DESTINATION ) {
            __ptr_acf->flight_plan.pop_front();
            wp = __ptr_acf->flight_plan.get(0);
        };
        
        auto our_location = __ptr_acf->get_location();    
        auto our_heading = __ptr_acf->get_rotation().heading;
    
        auto destination = __ptr_acf->flight_plan.destination();
        
        if ( destination.empty() ) {
            Logger::log("AgentAircraft::state_changed, landed: FP without destination.");
            return;
        }
        
        auto airport = Airport::get_by_icao( destination );

        if ( __ptr_acf->acf_condition.icao_type.empty() ) {
            Logger::log("AgentAircraft::state_changed, landed, aircraft ICAO type empty.");
            return;
        };
        
        startup_location_t parking;
        
        if ( __ptr_acf->agent_uuid() == B738_AFF ) parking = airport.get_startup_locations()["15"];
        else if ( __ptr_acf->agent_uuid() == B738_SBI ) parking = airport.get_startup_locations()["14"];
        else if ( __ptr_acf->agent_uuid() == A321_AFL ) parking = airport.get_startup_locations()["13"];
        else if ( __ptr_acf->agent_uuid() == A321_SVR ) parking = airport.get_startup_locations()["12"];
        else if ( __ptr_acf->agent_uuid() == B772_UAE ) parking = airport.get_startup_locations()["10"];
        else if ( __ptr_acf->agent_uuid() == B744_SWI ) parking = airport.get_startup_locations()["9"];
        else if ( __ptr_acf->agent_uuid() == B763_SAS ) parking = airport.get_startup_locations()["8"];
        else if ( __ptr_acf->agent_uuid() == B744_THA ) parking = airport.get_startup_locations()["7"];
        
        __ptr_acf->acf_condition.parking = parking.name;
        
        auto way = airport.get_taxi_way_for_parking( our_location, our_heading, parking );
        __ptr_acf->prepare_for_taxing( way );
                
        if (( _communicator ) && ( _communicator->is_connected() )) {
            CmdFlightPlan * cmd_flight_plan = new CmdFlightPlan( __ptr_acf->vcl_condition, __ptr_acf->flight_plan);
            _communicator->request( cmd_flight_plan );
        }
        
    };
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   В графе самолета началось новое действие                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::action_started( void * action ) {    
    scream_about_me();    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       В графе самолета закончилось действие и надо что-то делать дальше                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::action_finished( void * action ) {     
    __decision();
    scream_about_me();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *              Нулевая точка полетного плана была достигнута и удалена из самого полетного плана                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::wp_reached( waypoint_t wp ) {
    
    if ( _communicator && _communicator->is_connected() ) {
        CmdWaypointReached * cmd = new CmdWaypointReached( __ptr_acf->vcl_condition, wp.npp );
        _communicator->request( cmd );
    };
    
    auto next_wp = __ptr_acf->flight_plan.get(0);
    if ( next_wp.action_to_achieve == ACF_DOES_GLIDING ) {
        __ptr_acf->graph->set_active_state( ACF_STATE_APPROACH );
    }
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Перекрытая функция получения команды из сети                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_received( void * abstract_command ) {
        
    // Сначала вызываем родительский метод, он разложит по коллекции внешних агентов.
    AbstractAgent::on_received( abstract_command );
    
    AbstractCommand * cmd = ( AbstractCommand * ) abstract_command;
    
    CmdAircraftCondition * cmd_aircraft_condition = dynamic_cast< CmdAircraftCondition * > ( cmd );
    if ( cmd_aircraft_condition ) {
        on_received( cmd_aircraft_condition );      
        return;
    }
        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Из сети была получена команда состояния какого-то самолета.                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_received( CmdAircraftCondition * cmd ) {        
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Установлено соединение с коммуникатором                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_connect() {
    
    Logger::log("Communicator connected!");

    // Даем свое состояние коммуникатору.
    scream_about_me();    
    
    // Полетный план.
    CmdFlightPlan * cmd_flight_plan = new CmdFlightPlan( __ptr_acf->vcl_condition, __ptr_acf->flight_plan );
    _communicator->request( cmd_flight_plan );
    
    // Спрашиваем наше окружение - тех агентов, которых мы можем "слышать".
    CmdQueryAround * cmd_around = new CmdQueryAround( __ptr_acf->vcl_condition );
    _communicator->request( cmd_around );    

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Один "шаг"                                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__step() {    

    // Время, прошедшее с момента предыдущего вызова процедуры обновления.
    float elapsed_since_last_call = ( xenon::get_system_time_ms() - __previous_time) / 1000.0;
    __previous_time = xenon::get_system_time_ms();


    // Вызываем обновление состояния агента.
    __ptr_acf->UpdatePosition( elapsed_since_last_call, 0 );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Соединение с коммуникатором было разорвано                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_disconnect() {
    Logger::log("Communicator was disconnected...");
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  При попытке работы с сетью были получены ошибки                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_error( std::string message ) {
    Logger::log("Network error: " + message);
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Новый вариант принятия решения о дальнейшем шаге поведения                                *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__decision() {
        
    if ( ! __started ) return;
    
    // Текущее состояние.
    
    auto current_state_object = __ptr_acf->graph->get_current_state();
    auto node = __ptr_acf->graph->get_node_for( current_state_object );
    
    // Есть ли фреймы на данное состояние?
    if ( __state_frames.count( node.state )) {
        
        vector<next_action_t> result;
        
        for ( auto frame : __state_frames[ node.state ] ) {
            frame->update();
            if ( frame->activated() ) {
                next_action_t his_result;
                frame->result( his_result );
                if ( his_result.action != ACF_DOES_NOTHING ) result.push_back( his_result );
            }
        }
        
//         Logger::log( __ptr_acf->vcl_condition.agent_name + ", decision:" );
//         for ( auto r : result ) {
//             Logger::log( "    " + action_to_string( r.action ) + ", priority=" + to_string( r.priority ) );
//         };
                
        if ( result.empty() ) {
            // У-п-с. Не из чего выбирать. Ок, берем из полетного плана тогда.
            
            auto action = __ptr_acf->flight_plan.get(0).action_to_achieve;
            if ( ! __ptr_acf->graph->current_action_is( action ) ) __start_fp0_action();            
            
            return;
        };
        
        // Сортировка полученного результата по приоритетам.
        std::sort( result.begin(), result.end(), []( next_action_t & a, next_action_t & b ) {
            return ( a.priority > b.priority );
        });
        
        // Нулевой элемент - это следующее действие.
        auto next_action = result[0].action;
        // Фрейм - легко - мог ни разу не обновиться. В этом случае он выдаст ACF_DOES_NOTHING.
        if ( next_action == ACF_DOES_NOTHING ) next_action = __ptr_acf->flight_plan.get(0).action_to_achieve;
        
        // Возможно, именно оно сейчас и выполняется, тогда ничего не делаем.
        if ( __ptr_acf->graph->current_action_is( next_action )) {
            return;
        }
        
        // Будем переставлять. Правда, тут еще есть вопрос: а доступно ли оно из текущего состояния?
        aircraft_state_graph::graph_t::edge_descriptor fake;
        
        auto action_descriptor = __ptr_acf->graph->get_action_outgoing_from_current_state( next_action );
        
        if ( action_descriptor == fake ) {
            Logger::log(
                "AgentAircraft::__decision, got fake descriptor. State=" + node.name 
                + ", next action=" + to_string(next_action)
            );
            return;
        }
        
        try {
            __ptr_acf->graph->set_active_action( action_descriptor );
        } catch ( const std::runtime_error & e ) {
            Logger::log("AgentAircraft::__decision: no action " + to_string(next_action) + " from node " + node.name );            
        }
        
    } else {
        // Logger::log("No frames for " + node.name );
        // Если фреймов нет, то пока что запускаем действие, предусмотренное фронт-точкой полетного плана.
        
        auto action = __ptr_acf->flight_plan.get(0).action_to_achieve;
        if ( ! __ptr_acf->graph->current_action_is( action ) ) __start_fp0_action();
        
    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Старт действия, предусмотренного следующей точкой полетного плана                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__start_fp0_action() {
        
    auto next_wp = __ptr_acf->flight_plan.get(0);
    if ( next_wp.action_to_achieve == ACF_DOES_NOTHING ) {
        Logger::log(
            "AgentAircraft::__start_fp0_action: wp: " + next_wp.name + ", "
            + waypoint_to_string( next_wp.type ) 
            + ", nothing to do."
        );
        return;
    }        
    
    auto current_state = __ptr_acf->graph->get_current_state();
    auto node = __ptr_acf->graph->get_node_for( current_state );
    
    if ( node.state == ACF_STATE_PARKING && 
        ( 
            next_wp.action_to_achieve == ACF_DOES_PUSH_BACK 
            || next_wp.action_to_achieve == ACF_DOES_SLOW_TAXING
        )
    ) {
        // Мы еще не сдвинулись с парковки. А - собрались. Сдвигаемся принудительно,
        // с парковки ушли, дальше пойдет либо выталкивание, либо медленное выруливание.
        __ptr_acf->graph->set_active_state( ACF_STATE_MOTION_STARTED );
    };
        
    if ( node.state == ACF_STATE_READY_FOR_TAXING && next_wp.action_to_achieve == ACF_DOES_LINING_UP ) {
        // Мы не доехали до HP. HP - это не точка, это расстояние. 
        // Соответственно, состояние еще не изменилось.
        // Значит, продолжаем руление.
        next_wp.action_to_achieve = ACF_DOES_NORMAL_TAXING;
    };
    
    if ( node.state == ACF_STATE_HP && next_wp.action_to_achieve == ACF_DOES_TAKE_OFF ) {
        // Не закончилась фаза выравнивания.
        next_wp.action_to_achieve = ACF_DOES_LINING_UP;
    };    
    
    
    if ( __ptr_acf->graph->current_action_is( next_wp.action_to_achieve )) return;
    
    aircraft_state_graph::graph_t::edge_descriptor fake;
    try {
        aircraft_state_graph::graph_t::edge_descriptor action 
            = __ptr_acf->graph->get_action_outgoing_from_current_state( next_wp.action_to_achieve );
            
        if ( action == fake ) {
            Logger::log(
                "AgentAircraft: __start_fp0_action got fake edge descriptor. WP=" 
                + next_wp.name + ", type=" + waypoint_to_string(next_wp.type) 
                + ", State=" + node.name
                + ", action=" + action_to_string( next_wp.action_to_achieve)
            );
            return;
        }
        
        __ptr_acf->graph->set_active_action( action );

    } catch ( const std::range_error & re ) {
        
        Logger::log(
            "AgentAircraft::__start_fp0_action, invalid descriptor for action " 
            + to_string( next_wp.action_to_achieve ) 
            + " from state " + node.name
            + ", message=" + string( re.what() )
        );
    }

}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Деструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AgentAircraft::~AgentAircraft() {
    
}
