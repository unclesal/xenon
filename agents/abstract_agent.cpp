// *********************************************************************************************************************
// *         Любой внешний агент, который может общаться с коммуникатором и через него - с другими агентами            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:13 *
// *********************************************************************************************************************

#include "abstract_agent.h"

using namespace xenon;
using namespace std;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractAgent::AbstractAgent() {
    _communicator = nullptr;    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Из сети была получена команда                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractAgent::on_received( void * abstract_command ) {

    AbstractCommand * cmd = ( AbstractCommand * ) abstract_command;
    cmd->execute_on_agent( this );

};


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Деструктор                                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractAgent::~AbstractAgent() {
    
    if ( _communicator ) {
        if ( _communicator->is_connected() ) _communicator->disconnect();
        delete( _communicator );
        _communicator = nullptr;
    }
    
    for ( auto agent: agents ) {
        
        if ( agent.acf_condition ) {
            delete( agent.acf_condition );
            agent.acf_condition = nullptr;
        };
        
        if ( agent.flight_plan ) {
            delete ( agent.flight_plan );
            agent.flight_plan = nullptr;
        };

    }
}

