// *********************************************************************************************************************
// *                                                   Агент самолета                                                  *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:31 *
// *********************************************************************************************************************
#pragma once

#include <unistd.h>

#include <string>

#include "airport.h"
#include "connected_communicator.h"
#include "connected_communicator_reactor.h"
#include "bimbo_aircraft.h"

namespace xenon {
    
    class AgentAircraft : public ConnectedCommunicatorReactor {
        
        public:
            
            AgentAircraft( 
                const std::string & uuid
            );

            virtual ~AgentAircraft();
            
            void run();
            
            void on_connect() override;
            void on_disconnect() override;
            void on_received( AbstractCommand * cmd ) override;
            void on_error( std::string message ) override;
            
        protected:                        
            
        private:

            BimboAircraft * __ptr_acf;
            void __step();
            
            ConnectedCommunicator * __communicator;
            
    };
    
}; // namespace xenon
