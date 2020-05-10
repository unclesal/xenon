// *********************************************************************************************************************
// *                                             Состояние автопилота.                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 sep 2019 at 21:38 *
// *********************************************************************************************************************
#pragma once

#include "jsonable.h"

// Флаги состояния автопилота.
#define AUTOPILOT_STATE_FLAG__AUTOTHROTTLE_ENGAGE               0x00001
#define AUTOPILOT_STATE_FLAG__HEADING_HOLD_ENGAGE               0x00002
#define AUTOPILOT_STATE_FLAG__WING_LEVEL_ENGAGE                 0x00004
#define AUTOPILOT_STATE_FLAG__AIRSPEED_HOLD_WITH_PITCH_ENGAGE   0x00008
#define AUTOPILOT_STATE_FLAG__VVI_CLIMB_ENGAGE                  0x00010
#define AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ARMED               0x00020
#define AUTOPILOT_STATE_FLAG__FLIGHT_LEVEL_CHANGE_ENGAGE        0x00040
#define AUTOPILOT_STATE_FLAG__PITCH_SYNC_ENGAGE                 0x00080
#define AUTOPILOT_STATE_FLAG__HNAV_ARMED                        0x00100
#define AUTOPILOT_STATE_FLAG__HNAV_ENGAGED                      0x00200
#define AUTOPILOT_STATE_FLAG__GLIDESLOPE_ARMED                  0x00400
#define AUTOPILOT_STATE_FLAG__GLIDESLOPE_ENGAGED                0x00800
#define AUTOPILOT_STATE_FLAG__FMS_ARMED                         0x01000
#define AUTOPILOT_STATE_FLAG__FMS_ENGAGED                       0x02000
#define AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ENGAGED             0x04000
#define AUTOPILOT_STATE_FLAG__HORIZONTAL_TOGA_ENGAGED           0x08000
#define AUTOPILOT_STATE_FLAG__VERTICAL_TOGA_ENGAGED             0x10000
#define AUTOPILOT_STATE_FLAG__VNAV_ARMED                        0x20000
#define AUTOPILOT_STATE_FLAG__VNAV_ENGAGED                      0x40000

namespace xenon {

    class CommandAutopilotState : public JSONAble {

        public:

            static string COMMAND_NAME;

            CommandAutopilotState();
            virtual ~CommandAutopilotState() override = default;

            virtual void to_json( JSON & json ) override;
            virtual void from_json( JSON & json ) override;

            // Тип автопилота. Только для чтения, переставить его нельзя.
            enum AP_TYPE_T {
                AP_NONE = -1,
                AP_XP_CUSTOM = 0,
                AP_AIRLINER = 1,
                AP_GFC_700 = 2,
                AP_S_TEC_55 = 3,
                AP_S_TEC_WITH_ALTITUDE = 4,
                AP_KAP_140_SINGLE_AXIS = 5,
                AP_KAP_140_DUAL_AXIS = 6,
                AP_KAP_140_DUAL_WITH_ALTITUDE = 7,
                AP_PIPER = 8
            };

            enum AP_AH_SOURCE_T {
                AH_NONE = -1,
                AH_AHRS = 10,
                AH_ELEC_GYRO = 11,
                AH_VACUUM_GYRO = 12,
                AH_TURN_COORDINATOR_AND_ABSOLUTE_PRESSURE = 13
            };

            enum AP_DG_SOURCE_T {
                DG_NONE = -1,
                DG_AHRS = 10,
                DG_ELEC = 11,
                DG_VACUUM = 12
            };

            // Режим работы автопилота.
            enum AP_MODE_T {
                AP_MODE_NONE = -1,
                AP_MODE_OFF = 0,
                AP_MODE_FLIGHT_DIR = 1,
                AP_MODE_ON=2
            };

            // Режим работы дроссельного привода двигателей.
            // Auto-throttle: 0=servos declutched (arm, hold), 1=airspeed hold, 2=N1 target hold, 3=retard, 4=reserved for future use
            enum AP_THROTTLE_T {
                THROTTLE_UNKNOWN = -1,
                THROTTLE_SERVOS = 0,
                THROTTLE_AIRSPEED_HOLD = 1,
                THROTTLE_N1 = 2,
                THROTTLE_RETARD = 3,
                THROTTLE_RESERVED = 4
            };

            // Вид селектора подхода, Nav1 или Nav2.
            /*
            enum AP_APPROACH_SELECTOR_T {
                AP_APPROACH_SELECTOR_NONE = -1,
                AP_APPROACH_SELECTOR_NAV1 = 0,
                AP_APPROACH_SELECTOR_NAV2 = 1
            };
            */

            float heading() { return _heading; }
            float heading_mag() { return _heading_mag; }
            float altitude() { return _altitude; }
            float vertical_velocity() { return _vertical_velocity; }
            float airspeed() { return _airspeed; }
            AP_MODE_T autopilot_mode() { return _autopilot_mode; }
            int autopilot_state() { return _autopilot_state; }

            AP_THROTTLE_T autothrottle() { return _autothrottle; }
            bool autothrottle_on() { return static_cast<bool>(_autothrottle_on); }

        protected:

            // Step increment for autopilot VVI
            float _vvi_step_ft;

            // Step increment for autopilot altitude
            float _alt_step_ft;

            // Step increment for radio altimeter decision height
            float _radio_altimeter_step_ft;

            // 0=X-Plane custom, 1=Airliner, 2=GFC-700, 3=S-Tec 55, 4=S-Tec 55 with altitude preselect,
            // 5=KAP-140 single axis, 6=KAP-140 dual axis, 7=KAP-140 dual axis with altitude preselect,
            // 8=Piper Autocontrol
            AP_TYPE_T _preconfigured_ap_type;

            // Whether the autopilot is strictly single axis (bank) only.
            bool _single_axis_autopilot;

            // Attitude source for the autopilot: 10 = AHRS, 11 = elec gyro, 12 = vacuum gyro,
            // 13 = turn coordinator and absolute pressure transducer
            AP_AH_SOURCE_T _ah_source;

            // Directional gyro source for the autopillot: 10 = AHRS, 11 = elec gyro (HSI or DG), 12 = vacuum gyro (DG)
            AP_DG_SOURCE_T _dg_source;

            // The autopilot master mode (off=0, flight director=1, on=2)
            AP_MODE_T _autopilot_mode;

            // Airspeed mode for the autopilot. DEPRECATED
            int _airspeed_mode;

            // Nav1 or Nav2 approach selection
            // Не работает, закончился на версии 700
            // AP_APPROACH_SELECTOR_T _approach_selector;

            // Altitude dialed into the AP
            float _altitude;

            // Currently held altitude (remembered until you hit flchg)
            float _current_altitude;

            // Vertical speed to hold
            float _vertical_velocity;

            // 	Airspeed to hold, this changes from knots to a mach number
            float _airspeed;

            // The heading to fly (true, legacy)
            float _heading;

            // The heading to fly (magnetic, preferred) pilot
            float _heading_mag;

            // Various autopilot engage modes, etc. See docs for flags
            int _autopilot_state;

            // Auto-throttle: 0=servos declutched (arm, hold), 1=airspeed hold, 2=N1 target hold, 3=retard, 4=reserved for future use
            AP_THROTTLE_T _autothrottle;

            // Auto-throttle really working? Takes into account failures, esys, etc.
            int _autothrottle_on;

        private:
    };
};

