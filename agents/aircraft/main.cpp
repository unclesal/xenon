// *********************************************************************************************************************
// *                                              Запускашка агента самолета                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:22 *
// *********************************************************************************************************************

#include <iostream>
#include <thread>

#include "agent_aircraft.h"
#include "tested_agents.h"

using namespace std;
using namespace xenon;

int main( int argc, char * argv[] ) {

    AgentAircraft b738_aff( B738_AFF );
    // b738_aff.run();
    std::thread b738_aff_thread( &AgentAircraft::run, &b738_aff );
    b738_aff_thread.detach();

    AgentAircraft b738_sbi( B738_SBI );
    std::thread b738_sbi_thread( &AgentAircraft::run, &b738_sbi );
    b738_sbi_thread.detach();
    
    AgentAircraft a321_afl( A321_AFL );
    std::thread a321_afl_thread(  &AgentAircraft::run, &a321_afl );
    a321_afl_thread.detach();        
    
    AgentAircraft a321_svr ( A321_SVR );
    std::thread a321_svr_thread( &AgentAircraft::run, &a321_svr );
    a321_svr_thread.detach();

    AgentAircraft b772_uae( B772_UAE );
    std::thread b772_uae_thread( &AgentAircraft::run, &b772_uae );
    b772_uae_thread.detach();
    
    AgentAircraft b744_swi( B744_SWI );
    std::thread b744_swi_thread( &AgentAircraft::run, &b744_swi );
    b744_swi_thread.detach();

    AgentAircraft b744_tha( B744_THA );
    std::thread b744_tha_thread( & AgentAircraft::run, &b744_tha );
    b744_tha_thread.detach();
    
    AgentAircraft b763_sas( B763_SAS );
    b763_sas.run();

    return 0;
};
