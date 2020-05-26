// *********************************************************************************************************************
// *                    Класс, обеспечивающий общение агентов между собой - внутри симулятора X-Plane                  *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 26 may 2020 at 15:13 *
// *********************************************************************************************************************

#pragma once

#include "talker.h"
#include "xplane_communicator_interface.h"

namespace xenon {

    class XPlaneCommunicator : public XPlaneCommunicatorInterface {

        public:

            XPlaneCommunicator();
            virtual ~XPlaneCommunicator();

            void register_talker( Talker * talker );
            void unregister_talker( Talker * talker );

    };

}; // namespace xenon

