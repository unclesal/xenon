// *********************************************************************************************************************
// *                                          Команда передачи по сети полетного плана                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 09 jul 2020 at 09:29 *
// *********************************************************************************************************************
#pragma once
#include "abstract_command.h"
#include "flight_plan.h"

namespace xenon {
    
    class CmdFlightPlan : public AbstractCommand {
        
        friend class ConnectedClient;
    
        public:
            
            CmdFlightPlan();
            CmdFlightPlan(
                const vehicle_condition_t & vcl_condition, const FlightPlan & flight_plan
            );
            ~CmdFlightPlan() override = default;
            
            void to_json( JSON & json ) override;
            void from_json( JSON & json ) override;
            
            const FlightPlan & flight_plan() { return __fp; };
            
#ifdef SERVER_SIDE
            void execute_on_server( ConnectedClientCore * client, ClientsListener * server ) override;
#endif

#ifdef INSIDE_AGENT
            void execute_on_agent( ConnectedCommunicatorReactor * current_agent ) override;
#endif
            
        protected:
            
        private:
            
            FlightPlan __fp;
        
    }; // class CmdFlightPlan
    
}; // namespace xenon
