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
                
    AgentAircraft b738_aff( B738_AFF );    
    std::thread b738_aff_thread( &AgentAircraft::run, &b738_aff );
    b738_aff_thread.detach();
    
    AgentAircraft a321_afl( A321_AFL );
    std::thread a321_afl_thread(  &AgentAircraft::run, &a321_afl );
    a321_afl_thread.detach();        
    
    AgentAircraft a321_svr ( A321_SVR );
    std::thread a321_svr_thread( &AgentAircraft::run, &a321_svr );
    a321_svr_thread.detach();
    
    AgentAircraft b772_uae( B772_UAE );
    std::thread b772_uae_thread( &AgentAircraft::run, &b772_uae );
    b772_uae_thread.detach();
    
    AgentAircraft b744_sva( B744_SVA );
    std::thread b744_sva_thread( &AgentAircraft::run, &b744_sva );
    b744_sva_thread.detach();
    
    AgentAircraft b763_ely( B763_ELY );
    b763_ely.run();
    
    return 0;
};
