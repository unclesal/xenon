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
        
        public:
            
            AbstractCommand();
            virtual ~AbstractCommand() override = default;
            
            virtual void to_json(JSON & json) override;
            virtual void from_json( JSON & json ) override;

#ifdef SERVER_SIDE
            virtual void execute_on_server( ConnectedClientListener * client, ClientsListener * server ) = 0;
#endif

        protected:
            
        private:
            
            unsigned int __packet_number;
            std::string __agent_uuid;
            agent_t __agent_type;
            
    }; // AbstractCommand
    
}; // xenon
