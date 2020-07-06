// *********************************************************************************************************************
// *                                              Реализация агента самолета                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:49 *
// *********************************************************************************************************************
#include "agent_aircraft.h"
#include "tested_agents.h"
#include "cmd_query_around.h"
#include "cmd_aircraft_condition.h"

using namespace xenon;
using namespace std;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AgentAircraft::AgentAircraft ( const std::string & uuid ) : AbstractAgent() {
    
    __cycles = 0;
    
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
    
    // Коммуникатор порождаем последним, т.к. там потоки и он может тут же 
    // соединиться. Нужно, чтобы все указатели были уже инициализированы.
    _create_communicator();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Времянка: создание самолета по его UUIDу                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::__temporary_make_aircraft_by_uuid( const std::string & uuid ) {
    
    auto usss = Airport::get_by_icao("USSS");
    
    if ( uuid == B738_AFF ) {        
        
        __ptr_acf = new BimboAircraft("B738", "AFF", "AFF");
        __ptr_acf->vcl_condition.agent_name = "Boeing 737-800 AFF";
                
        auto gate = usss.get_startup_locations()["15"];
        __ptr_acf->place_on_ground( gate );

        // __ptr_acf->test__fly();
        // __ptr_acf->choose_next_action();
        
    } else if ( uuid == A321_AFL ) {
        
        __ptr_acf = new BimboAircraft("A321", "AFL", "AFL");
        __ptr_acf->vcl_condition.agent_name = "Airbus A321 AFL";
        
        
        auto gate = usss.get_startup_locations()["13"];
        __ptr_acf->place_on_ground( gate );
        
    } else if ( uuid == A321_SVR ) { 
        
        __ptr_acf = new BimboAircraft("A321", "SVR", "SVR");
        __ptr_acf->vcl_condition.agent_name = "Airbus A321 SVR";
        
        auto gate = usss.get_startup_locations()["12"];
        __ptr_acf->place_on_ground( gate );
        
    } else if ( uuid == B772_UAE ) {
        
        __ptr_acf = new BimboAircraft("B772", "UAE", "UAE");
        __ptr_acf->vcl_condition.agent_name = "Boeing 777-200 UAE";
        
        auto gate = usss.get_startup_locations()["11"];
        __ptr_acf->place_on_ground( gate );
    }
    
    if ( __ptr_acf ) {
        __ptr_acf->vcl_condition.agent_uuid = uuid;
        __ptr_acf->vcl_condition.agent_type = AGENT_AIRCRAFT;
        
        auto where_i_am = __ptr_acf->get_location();    
        auto way = usss.get_taxi_way_for_departure( where_i_am );        
        __ptr_acf->prepare_for_take_off( way );
        __ptr_acf->test__fly();
        __ptr_acf->choose_next_action();
    }

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
        __step();
        __cycles ++;
        
        if ( __cycles >= CYCLES_PER_CRY ) {
            __cycles = 0;
            
            if ( _communicator->is_connected() ) {
                CmdAircraftCondition * cmd = new CmdAircraftCondition(
                    __ptr_acf->vcl_condition, __ptr_acf->acf_condition
                );
                _communicator->request( cmd );
                Logger::log("New state sended for " + __ptr_acf->vcl_condition.agent_name);
            }
            
        }

    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Для отладки                                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

/*
void AgentAircraft::__test() {
    auto usss = Airport::get_by_icao("USSS");
    auto parking = usss.get_free_parking("B738");
    location_t start_point = {
        .latitude = 56.744801,
        .longitude = 60.803618,
        .altitude = 170.0
    };

    auto heading = 80.0;
    auto way = usss.get_taxi_way_for_parking( start_point, heading, parking );

    // Он запросто мог получиться нулевой длины, если самолет стоял как-нибудь "боком".
    if ( way.empty() ) {
        Logger::log("AgentAircraft::__test(), taxi way for parking is empty.");
        return;
    }
}
*/

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Установлено соединение с коммуникатором                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_connect() {

    // Даем свое состояние коммуникатору.
    CmdAircraftCondition * cmd_acf_condition = new CmdAircraftCondition(
        __ptr_acf->vcl_condition, __ptr_acf->acf_condition
    );
    _communicator->request( cmd_acf_condition );
    
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
    cout << "Communicator was disconnected..." << endl;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  При попытке работы с сетью были получены ошибки                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_error( std::string message ) {
    cout << "Network error: " << message << endl;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Деструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AgentAircraft::~AgentAircraft() {
    
}
