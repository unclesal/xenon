// *********************************************************************************************************************
// *                                    Внешний (по отношению к X-Plane) самолет.                                      *
// *      Класс становится базовым для BimboAircraft, если мы находимся снаружи X-Plane (вместо XPMP2::Aircraft)       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 may 2020 at 09:31 *
// *********************************************************************************************************************
#pragma once

#include <string>
#include <array>

#include "structures.h"

namespace xenon {

    // Его проще сюда передрать полностью, т.к. я использую не только
    // v_count, но и многие из этих определений. Не сильно хорошо, т.к.
    // придется следить за их соответствием с XPMP2 классом.
    /*

    enum DR_VALS {
        V_CONTROLS_GEAR_RATIO = 0,                  ///< `libxplanemp/controls/gear_ratio` and \n`sim/cockpit2/tcas/targets/position/gear_deploy`
        V_CONTROLS_FLAP_RATIO,                      ///< `libxplanemp/controls/flap_ratio` and \n`sim/cockpit2/tcas/targets/position/flap_ratio` and `...flap_ratio2`
        V_CONTROLS_SPOILER_RATIO,                   ///< `libxplanemp/controls/spoiler_ratio`
        V_CONTROLS_SPEED_BRAKE_RATIO,               ///< `libxplanemp/controls/speed_brake_ratio` and \n`sim/cockpit2/tcas/targets/position/speedbrake_ratio`
        V_CONTROLS_SLAT_RATIO,                      ///< `libxplanemp/controls/slat_ratio` and \n`sim/cockpit2/tcas/targets/position/slat_ratio`
        V_CONTROLS_WING_SWEEP_RATIO,                ///< `libxplanemp/controls/wing_sweep_ratio` and \n`sim/cockpit2/tcas/targets/position/wing_sweep`
        V_CONTROLS_THRUST_RATIO,                    ///< `libxplanemp/controls/thrust_ratio` and \n`sim/cockpit2/tcas/targets/position/throttle`
        V_CONTROLS_YOKE_PITCH_RATIO,                ///< `libxplanemp/controls/yoke_pitch_ratio` and \n`sim/cockpit2/tcas/targets/position/yolk_pitch`
        V_CONTROLS_YOKE_HEADING_RATIO,              ///< `libxplanemp/controls/yoke_heading_ratio` and \n`sim/cockpit2/tcas/targets/position/yolk_yaw`
        V_CONTROLS_YOKE_ROLL_RATIO,                 ///< `libxplanemp/controls/yoke_roll_ratio` and \n`sim/cockpit2/tcas/targets/position/yolk_roll`
        V_CONTROLS_THRUST_REVERS,                   ///< `libxplanemp/controls/thrust_revers`

        V_CONTROLS_TAXI_LITES_ON,                   ///< `libxplanemp/controls/taxi_lites_on` and \n`sim/cockpit2/tcas/targets/position/lights`
        V_CONTROLS_LANDING_LITES_ON,                ///< `libxplanemp/controls/landing_lites_on` and \n`sim/cockpit2/tcas/targets/position/lights`
        V_CONTROLS_BEACON_LITES_ON,                 ///< `libxplanemp/controls/beacon_lites_on` and \n`sim/cockpit2/tcas/targets/position/lights`
        V_CONTROLS_STROBE_LITES_ON,                 ///< `libxplanemp/controls/strobe_lites_on` and \n`sim/cockpit2/tcas/targets/position/lights`
        V_CONTROLS_NAV_LITES_ON,                    ///< `libxplanemp/controls/nav_lites_on` and \n`sim/cockpit2/tcas/targets/position/lights`

        V_GEAR_TIRE_VERTICAL_DEFLECTION_MTR,        ///< `libxplanemp/gear/tire_vertical_deflection_mtr`
        V_GEAR_TIRE_ROTATION_ANGLE_DEG,             ///< `libxplanemp/gear/tire_rotation_angle_deg`
        V_GEAR_TIRE_ROTATION_SPEED_RPM,             ///< `libxplanemp/gear/tire_rotation_speed_rpm`
        V_GEAR_TIRE_ROTATION_SPEED_RAD_SEC,         ///< `libxplanemp/gear/tire_rotation_speed_rad_sec`

        V_ENGINES_ENGINE_ROTATION_ANGLE_DEG,        ///< `libxplanemp/engines/engine_rotation_angle_deg`
        V_ENGINES_ENGINE_ROTATION_SPEED_RPM,        ///< `libxplanemp/engines/engine_rotation_speed_rpm`
        V_ENGINES_ENGINE_ROTATION_SPEED_RAD_SEC,    ///< `libxplanemp/engines/engine_rotation_speed_rad_sec`
        V_ENGINES_PROP_ROTATION_ANGLE_DEG,          ///< `libxplanemp/engines/prop_rotation_angle_deg`
        V_ENGINES_PROP_ROTATION_SPEED_RPM,          ///< `libxplanemp/engines/prop_rotation_speed_rpm`
        V_ENGINES_PROP_ROTATION_SPEED_RAD_SEC,      ///< `libxplanemp/engines/prop_rotation_speed_rad_sec`
        V_ENGINES_THRUST_REVERSER_DEPLOY_RATIO,     ///< `libxplanemp/engines/thrust_reverser_deploy_ratio`

        V_MISC_TOUCH_DOWN,                          ///< `libxplanemp/misc/touch_down`

        V_COUNT                                     ///< always last, number of dataRefs supported
    };
    */

    class ExternalAircraft {

        public:

            ExternalAircraft(
                const std::string & icao_type, const std::string & icao_airline, const std::string & livery
            );
            virtual ~ExternalAircraft() = default;

            // Имена сохранены такие же, как в XPMP2::Aircraft

            // std::array<float,V_COUNT> v;

            virtual void UpdatePosition(float elapsed_since_last_call, int fl_counter);

            // Имена сохранены, что позволяет в Bimbo вызывать эти функции.


        protected:

            // rotation_t _rotation;

        private:

    };

}; // xenon
