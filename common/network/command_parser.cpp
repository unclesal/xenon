// *********************************************************************************************************************
// *                                      Распознавание полученных по сети команд                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 07:05 *
// *********************************************************************************************************************
#include "command_parser.h"

#include "cmd_aircraft_condition.h"
#include "cmd_query_around.h"
#include "cmd_vehicle_condition.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

CommandParser::CommandParser() {
    __register< CmdVehicleCondition >( "CmdVehicleCondition" );
    __register< CmdQueryAround >( "CmdQueryAround" );
    __register< CmdAircraftCondition >( "CmdAircraftCondition" );    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Попытка понять, что пришло по сети                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractCommand * CommandParser::parse( char * buffer, const ssize_t & len, std::string & command_name ) {
    command_name = "";
    if ( ! len ) return nullptr;
    // Здесь пока ничего лучшего не придумал.
    vector <std::uint8_t> v_ubjson;
    for ( int i=0; i<len; ++i ) {
        v_ubjson.push_back( buffer[i] );
    }
    try {
        
        JSON json = JSON::from_ubjson(v_ubjson);
        command_name = json.value("command_name", "");
        Logger::log("Command name=" + command_name );
        auto instance = __create_instance( command_name );
        Logger::log("instance created");
        if ( instance ) instance->from_json( json );
        Logger::log("made from json");
        return instance;
        
    } catch ( JSON::parse_error & err ) {
    } catch ( const std::runtime_error & re ) {
    } catch ( ... ) {
    }
    return nullptr;
}
