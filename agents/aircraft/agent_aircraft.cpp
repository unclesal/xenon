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
    
    if ( ! Airport::airports_was_readed() ) {
        Airport::read_all();
    }

   // Ждем, пока аэропорт не вычитает свои данные.
   while ( ! Airport::airports_was_readed() ) {
       usleep(50);
   }

    if ( uuid == BOEING_1 ) {        
        
        __ptr_acf = new BimboAircraft("B738", "AFF", "AFF");
        __ptr_acf->vcl_condition.agent_name = "Agent 1";
        __ptr_acf->vcl_condition.agent_type = AGENT_AIRCRAFT;
        
        if ( ! Airport::airports_was_readed() ) {
            cerr << "Airports was not readed, exit." << endl;
            return;        
        } else {
            // Пока что все вручную и для отладки.
            auto usss = Airport::get_by_icao("USSS");
            auto gate = usss.get_startup_locations()["15"];    
            __ptr_acf->place_on_ground( gate );        
        }        
    }        
    
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
// *                                           Метод выполнения агента самолета                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::run() {
    
    if ( ! __ptr_acf ) {
        cerr << "AgentAircraft::run(): acf pointer is none. Exit." << endl;
        return;
    }
    
    for (;;) {
        sleep(100);
        // __step();
    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Установлено соединение с коммуникатором                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AgentAircraft::on_connect() {

    cout << "Communicator - connected!!!" << endl;
        
    Logger::log("Before send lat=" + to_string( __ptr_acf->vcl_condition.location.latitude)
        + ", lon=" + to_string( __ptr_acf->vcl_condition.location.longitude )
        + ", alt=" + to_string( __ptr_acf->vcl_condition.location.altitude )
        + ", clamped=" + to_string( __ptr_acf->vcl_condition.is_clamped_to_ground )
    );
    
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
