// *********************************************************************************************************************
// *                                      The common predecessor of all aircraft.                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 15:20 *
// *********************************************************************************************************************

#pragma once

// my own include files.
#include "jsonable.h"

namespace xenon {

    class AircraftState : public JSONAble {

        public:

            static string COMMAND_NAME;

            enum HSI_SELECTOR_T {
                HSI_UNKNOWN = -1,
                HSI_NAV_1   = 0,
                HSI_NAV_2   = 1,
                HSI_GPS     = 2
            };

            AircraftState(int acf_index = -1);
            AircraftState(const AircraftState & as);
            virtual ~AircraftState() override = default;

            virtual void to_json( JSON & json ) override;
            virtual void from_json( JSON & json ) override;

            int aircraft_index() { return _aircraft_index; }

            double latitude() { return _latitude; }
            double longitude() { return _longitude; }
            double altitude() { return _altitude; }

            float pitch() { return _pitch; }
            float roll() { return _roll; }
            float yaw() { return _yaw; }

            float heading() { return _heading; }

            float vvi() { return _vvi; }
            float ias() { return _ias; }
            float magnetic_variation() { return _magnetic_variation; }
            HSI_SELECTOR_T hsi_selector() { return _hsi_selector; }

        protected:

            int _aircraft_index;

            double _latitude;
            double _longitude;
            double _altitude;

            float _pitch;
            float _heading;
            float _roll;
            float _yaw;

            // Вертикальная скорость
            float _vvi;

            // Инструментальная скорость относительно воздуха.
            float _ias;
            // Локальное магнитное отклонение
            float _magnetic_variation;
            // GPS / NAV1 / NAV2
            HSI_SELECTOR_T _hsi_selector;

        private:



    };

};

