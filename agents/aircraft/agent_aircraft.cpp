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
#include "push_back_allowed.h"

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
        __start_time += 10000;
        
    } else if ( uuid == A321_AFL ) {
        
        __ptr_acf = new BimboAircraft("A321", "AFL", "AFL");
        __ptr_acf->vcl_condition.agent_name = "Airbus A321 AFL";                
        gate = usss.get_startup_locations()["13"];
        __start_time += 20000;
        
        
    } else if ( uuid == A321_SVR ) { 
        
        __ptr_acf = new BimboAircraft("A321", "SVR", "SVR");
        __ptr_acf->vcl_condition.agent_name = "Airbus A321 SVR";                
        gate = usss.get_startup_locations()["12"];        
        __start_time += 30000;
        
    } else if ( uuid == B772_UAE ) {
        
        __ptr_acf = new BimboAircraft("B772", "UAE", "UAE");
        __ptr_acf->vcl_condition.agent_name = "Boeing 777-200 UAE";        
        gate = usss.get_startup_locations()["11"];        
        __start_time += 40000;
    }
    
    if ( __ptr_acf ) {
        __ptr_acf->vcl_condition.agent_uuid = uuid;
        __ptr_acf->vcl_condition.agent_type = AGENT_AIRCRAFT;                
                                       
        __ptr_acf->place_on_ground( gate );
        
        auto where_i_am = __ptr_acf->get_location();    
        auto way = usss.get_taxi_way_for_departure( where_i_am );        
        __ptr_acf->prepare_for_take_off( way );
        __ptr_acf->test__fly();    
                
    }
    
    // Это - тоже времянка. Я их пока что запускаю всех вместе и получается
    // полная глупость, когда стартуют прям с точностью до милисекунды одновременно.
    int r = rand();    
    float rr = r * 100000.0 / (float) RAND_MAX;
    usleep((int)rr);
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

    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Выбор следующего действия                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

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
    cout << "AgentAircraft::state_changed" << endl;
    scream_about_me();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   В графе самолета началось новое действие                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::action_started( void * action ) {
    cout << "AgentAircraft::Action started" << endl;
    scream_about_me();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       В графе самолета закончилось действие и надо что-то делать дальше                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::action_finished( void * action ) {
    if ( __started ) __decision();
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
        
    auto current_state_object = __ptr_acf->graph->get_current_state();
    auto node = __ptr_acf->graph->get_node_for( current_state_object );
    
    // Есть ли в коллекции фреймов состояний - данное текущее состояние?
    if ( __state_frames.count( node.state )) {
        // Если оно есть, то проходимся по всем фреймам и обновляем их.
        for ( auto frame: __state_frames[ node.state ] ) {
            frame->update( cmd );
        }
    }
    
    if ( __started ) __decision();
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Установлено соединение с коммуникатором                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_connect() {

    // Даем свое состояние коммуникатору.
    scream_about_me();    
    
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
    
    // Текущее состояние.
    
    auto current_state_object = __ptr_acf->graph->get_current_state();
    auto node = __ptr_acf->graph->get_node_for( current_state_object );
    
    if ( __state_frames.count( node.state )) {
        
        vector<next_action_t> result;
        for ( auto frame: __state_frames[ node.state ] ) {
            next_action_t his_result;
            frame->result( his_result );
            result.push_back( his_result );
        }
                
        if ( result.empty() ) {
            // У-п-с. Не из чего выбирать.
            Logger::log("AgentAircraft::__decision(), nothing to select for " + node.name );            
            return;
        };
        
        // Сортировка полученного результата по вероятностям.
        sort( result.begin(), result.end(), []( next_action_t & a, next_action_t & b ) {
            return ( a.likeliness > b.likeliness );
        });
        
        // Нулевой элемент - это следующее действие.
        auto next_action = result[0].action;                
        
        // Возможно, именно оно сейчас и выполняется, тогда ничего не делаем.
        if ( __ptr_acf->graph->current_action_is( next_action )) return;
        
        // Будем переставлять. Правда, тут еще есть вопрос: а доступно ли оно из текущего состояния?
        auto action_descriptor = __ptr_acf->graph->get_action_outgoing_from_current_state( next_action );
        try {
            __ptr_acf->graph->set_active_action( action_descriptor );
        } catch ( const std::runtime_error & e ) {
            Logger::log("AgentAircraft::__decision: no action " + to_string(next_action) + " from node " + node.name );            
        }
        
    } else {
        Logger::log("AgentAircraft::__decision(): no frames found for " + node.name );
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Деструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AgentAircraft::~AgentAircraft() {
    
}
