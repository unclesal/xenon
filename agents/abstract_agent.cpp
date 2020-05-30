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

AbstractAgent::AbstractAgent( 
    const std::string & uuid, 
    const agent_t & agent_type, 
    const std::string & agent_name 
) {

    _communicator = new ConnectedCommunicator( this, agent_type, uuid, agent_name );

    if ( ! Airport::airports_was_readed() ) {        
        Airport::read_all();
    }

    // Ждем, пока аэропорт не вычитает свои данные.
    cout << "Waiting airports readed..." << endl;
    while ( ! Airport::airports_was_readed() ) {
        usleep(50);
    }
    cout << "apts readed, ok." << endl;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Деструктор                                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractAgent::~AbstractAgent() {
    if ( _communicator ) {
        delete( _communicator );
        _communicator = nullptr;
    }
}

