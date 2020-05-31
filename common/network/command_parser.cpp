// *********************************************************************************************************************
// *                                      Распознавание полученных по сети команд                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 07:05 *
// *********************************************************************************************************************
#include "command_parser.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

CommandParser::CommandParser() {
    __register< CmdHello >( "CmdHello" );
    __register< CmdVehicleCondition >( "CmdVehicleCondition" );
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
        return __create_instance( command_name );
        
    } catch ( JSON::parse_error & err ) {
    } catch ( const std::runtime_error & re ) {
    } catch ( ... ) {
    }
    return nullptr;
}
