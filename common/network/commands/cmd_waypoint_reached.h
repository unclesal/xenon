// *********************************************************************************************************************
// *                      В агенте (т.е. в "основной сущности") была достигнута точка полетного плана.                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 jul 2020 at 11:27 *
// *********************************************************************************************************************
#pragma once

#include "abstract_command.h"

namespace xenon {
    class CmdWaypointReached : public AbstractCommand {
    
        public:
            
            CmdWaypointReached();
            CmdWaypointReached(
                const vehicle_condition_t & vcl_condition,
                const uint16_t & npp
            );
            ~CmdWaypointReached() override = default;
            
            virtual void to_json( JSON & json ) override;
            virtual void from_json( JSON & json ) override;
            
            const uint16_t & npp() { return __npp; };
            
#ifdef SERVER_SIDE
            void execute_on_server( ConnectedClientCore * client, ClientsListener * server ) override;            
#endif
            
#ifdef INSIDE_AGENT
            void execute_on_agent( ConnectedCommunicatorReactor * current_agent ) override;
#endif
            
            
        protected:
            
        private:
            
            uint16_t __npp;
        
    };
};
