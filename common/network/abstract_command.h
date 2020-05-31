// *********************************************************************************************************************
// *                                    Общий предок всех передаваемых по сети команд (v2.0)                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 07:02 *
// *********************************************************************************************************************
#pragma once

#include "jsonable.h"
#include "structures.h"

#ifdef SERVER_SIDE
#include "connected_client_listener.h"
#include "clients_listener.h"
#endif

namespace xenon {
    
    class AbstractCommand : public JSONAble {
        
        friend class ConnectedCommunicator;
        friend class ConnectedClient;
        
        public:
            
            AbstractCommand();
            AbstractCommand( const say_to_t & say_to, const vehicle_condition_t & vcl_condition );
            virtual ~AbstractCommand() override = default;
            
            virtual void to_json(JSON & json) override;
            virtual void from_json( JSON & json ) override;
            
            const location_t & get_location() {
                return _vcl_condition.location;
            };
            
            const agent_t & get_agent_type() {
                return _vcl_condition.agent_type;
            };
            
            const std::string & get_agent_uuid() {
                return _vcl_condition.agent_uuid;
            };
            
            const std::string & get_to_agent_uuid() {
                return _vcl_condition.to_agent_uuid;
            };
            
            const say_to_t & sayed_to() {
                return _say_to;
            };

#ifdef SERVER_SIDE
            virtual void execute_on_server( ConnectedClientListener * client, ClientsListener * server );
#endif

        protected:
            
            vehicle_condition_t _vcl_condition;
            
        private:
            
            unsigned int __packet_number;
            
    }; // AbstractCommand
    
}; // xenon
