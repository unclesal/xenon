// *********************************************************************************************************************
// *                                       Команда передачи по сети состояния самолета                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 30 may 2020 at 15:12 *
// *********************************************************************************************************************
#pragma once

#include "cmd_vehicle_condition.h"

namespace xenon {
    
    class CmdAircraftCondition : public CmdVehicleCondition {
        
        friend class ConnectedClient;
        
        public:
            
            CmdAircraftCondition();
            CmdAircraftCondition( 
                const vehicle_condition_t & vcl_condition, const aircraft_condition_t & acf_condition 
            );
            
            ~CmdAircraftCondition() override = default;
            
            void to_json( JSON & json ) override;
            void from_json( JSON & json ) override;
            
#ifdef SERVER_SIDE
            void execute_on_server( ConnectedClientCore * client, ClientsListener * server ) override;
#endif

                        
        protected:
            
            aircraft_condition_t _acf_condition;
            
        private:
        
    }; // CmdAircraftCondition
    
}; // namespace xenon
