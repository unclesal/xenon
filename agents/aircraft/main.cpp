// *********************************************************************************************************************
// *                                              Запускашка агента самолета                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:22 *
// *********************************************************************************************************************

#include <iostream>
#include <thread>

#include "agent_aircraft.h"

using namespace std;
using namespace xenon;

int main( int argc, char * argv[] ) {
    
//     if (argc < 2 ) {        
//         cerr << "UUID parameter error. Usage: ./agent_aircraft acf_uuid" << endl;
//         return -1;
//     };
//     std::string uuid(argv[1]);
    
    std::string uuid("4204f982a17811eaaf3794de807942f4");
    AgentAircraft acf( uuid );
    acf.run();
    
};
