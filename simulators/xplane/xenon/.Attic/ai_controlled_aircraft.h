// *********************************************************************************************************************
// *               Самолет внутри X-Plane, не пользовательский, но которым мы можем управлять (AI traffic).            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 30 apr 2020 at 12:44 *
// *********************************************************************************************************************

#pragma once

#include <string>

#include "abstract_aircraft.h"

using namespace std;
namespace xenon {

    class AIControlledAircraft : public AbstractAircraft {

        public:

            AIControlledAircraft(int acf_index = -1);
            virtual ~AIControlledAircraft();

            void control( float elapsed_since_last_call );

        private:

            /**
             * @short Указатель на элемент автопилота данного AI-самолета.
             */
            static XPLMDataRef __dataref_autopilot;

    }; // ControlledAircraft

}; // namespace xenon
