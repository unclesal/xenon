// *********************************************************************************************************************
// *         Любой внешний агент, который может общаться с коммуникатором и через него - с другими агентами            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 05:51 *
// *********************************************************************************************************************
#pragma once

#include <string>

#include "structures.h"
#include "connected_communicator.h"
#include "connected_communicator_reactor.h"

namespace xenon {
    
    class AbstractAgent : public ConnectedCommunicatorReactor {
        
        public:
            
            AbstractAgent( const std::string & uuid, const agent_t & agent_type );
            virtual ~AbstractAgent();
            
            virtual void run() = 0;
            
        protected:
            
            ConnectedCommunicator * _communicator;
            
        private:
            
    }; // AbstractAgent
    
}; // xenon
