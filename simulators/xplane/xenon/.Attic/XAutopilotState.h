// *********************************************************************************************************************
// *                         Состояние автопилота, класс, который внутри симулятора X-Plane.                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 10:41 *
// *********************************************************************************************************************

#pragma once

#include "XPLMDataAccess.h"

#include "CommandAutopilotState.h"
#include "CommandSet.h"
#include "xplane_utilities.h"
#include "abstract_observed_object.h"

namespace xenon {
    
    class XAutopilotState : public CommandAutopilotState, public AbstractObservedObject {
        
        public:
            
            XAutopilotState();
            virtual ~XAutopilotState() override = default;
            
            virtual void observe();
            void set( CommandSet & cmd );
        
        protected:
        
        private:
            
            char __out[512];
            
            XPLMDataRef __vvi_step_ft_dataref;
            XPLMDataRef __alt_step_ft_dataref;
            XPLMDataRef __radio_altimeter_step_ft_dataref;
            XPLMDataRef __preconfigured_ap_type_dataref;
            XPLMDataRef __single_axis_autopilot_dataref;
            XPLMDataRef __ah_source_dataref;
            XPLMDataRef __dg_source_dataref;
            
            XPLMDataRef __autopilot_mode_dataref;
            
            // Airspeed mode for the autopilot. DEPRECATED
            XPLMDataRef __airspeed_mode_dataref;
            
            // XPLMDataRef __approach_selector_dataref;
            
            XPLMDataRef __altitude_dataref;
            XPLMDataRef __current_altitude_dataref;
            XPLMDataRef __vertical_velocity_dataref;
            
            // Airspeed to hold, this changes from knots to a mach number
            XPLMDataRef __airspeed_dataref;
            
            // The heading to fly (true, legacy)
            XPLMDataRef __heading_dataref;
            // The heading to fly (magnetic, preferred) pilot
            XPLMDataRef __heading_mag_dataref;
            
            // Various autopilot engage modes, etc. See docs for flags
            XPLMDataRef __autopilot_state_dataref;
            
            // Autothrottle
            XPLMDataRef __autothrottle_dataref;
            XPLMDataRef __autothrottle_on_dataref;                        
        
    };
};
