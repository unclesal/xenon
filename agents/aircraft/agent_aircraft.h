// *********************************************************************************************************************
// *                                                   Агент самолета                                                  *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:31 *
// *********************************************************************************************************************
#pragma once

#include <unistd.h>

#include <string>

#include "abstract_agent.h"
#include "bimbo_aircraft.h"

namespace xenon {
    
    class AgentAircraft : public AbstractAgent {
        
        public:
            
            AgentAircraft(const std::string & uuid);
            ~AgentAircraft() override = default;
            
            void run() override;
            
            void on_connect() override;
            void on_disconnect() override;
            void on_received( AbstractCommand * cmd ) override;
            void on_error( std::string message ) override;
            
        protected:                        
            
        private:

            BimboAircraft * __ptr_acf;
            void __step();
            
    };
    
}; // namespace xenon
