// *********************************************************************************************************************
// *                                              Запускашка агента самолета                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:22 *
// *********************************************************************************************************************

#include <iostream>
#include <thread>

#include "agent_aircraft.h"
#include "connected_communicator.h"
#include "tested_agents.h"

using namespace std;
using namespace xenon;

int main( int argc, char * argv[] ) {
    
//     if (argc < 2 ) {        
//         cerr << "UUID parameter error. Usage: ./agent_aircraft acf_uuid" << endl;
//         return -1;
//     };
//     std::string uuid(argv[1]);
    
    std::string uuid( BOEING_1 );    
    AgentAircraft acf( uuid );
    acf.run();
    
};
