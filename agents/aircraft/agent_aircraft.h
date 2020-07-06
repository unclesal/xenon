// *********************************************************************************************************************
// *                                                   Агент самолета                                                  *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:31 *
// *********************************************************************************************************************
#pragma once

#include <unistd.h>

#include <string>

#include "airport.h"
#include "abstract_agent.h"
#include "bimbo_aircraft.h"

namespace xenon {
    
    class AgentAircraft : public AbstractAgent {
        
        public:
            
            AgentAircraft( 
                const std::string & uuid
            );

            virtual ~AgentAircraft();
            
            void run();
            
            void on_connect() override;
            void on_disconnect() override;            
            
            void on_error( std::string message ) override;
            
        protected:                        
            
        private:

            long int __previous_time;
            unsigned int __cycles;

            BimboAircraft * __ptr_acf;
            void __step();

            // void __test();
            void __temporary_make_aircraft_by_uuid( const std::string & uuid );
            
    };
    
}; // namespace xenon
