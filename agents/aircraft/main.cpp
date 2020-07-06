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
        
//     AgentAircraft b738_aff( B738_AFF );
//     std::thread b738_aff_thread( &AgentAircraft::run, &b738_aff );
//     b738_aff_thread.detach();
//     
//     AgentAircraft a321_afl( A321_AFL );
//     std::thread a321_afl_thread(  &AgentAircraft::run, &a321_afl );
//     a321_afl_thread.detach();
    
    AgentAircraft a321_svr ( A321_SVR );
    a321_svr.run();
    
    return 0;
};
