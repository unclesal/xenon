// *********************************************************************************************************************
// *               Самолет внутри X-Plane, не пользовательский, но которым мы можем управлять (AI traffic).            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 30 apr 2020 at 13:14 *
// *********************************************************************************************************************

#include "XPLMPlanes.h"
#include "ai_controlled_aircraft.h"

using namespace xenon;
using namespace std;

const char * __AI_AUTOPILOT_PATH = "sim/operation/override/override_plane_ai_autopilot";

XPLMDataRef AIControlledAircraft::__dataref_autopilot = nullptr;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор.                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AIControlledAircraft::AIControlledAircraft(int acf_index)
    : AbstractAircraft(acf_index)
{

    // Снимаем управление данным самолетом - со стороны X-Plane.
    // XPLMDisableAIForPlane( _aircraft_index );

    /*
    // Попытка инициализации указателя на данные автопилота. Он один на весь класс,
    // так что его инициализируем - только в том случае, если там еще ничего нет.

    if ( ! __dataref_autopilot ) {
        __dataref_autopilot = XPlaneUtilities::find_data_ref( __AI_AUTOPILOT_PATH );
    };

    // Если указатель на автопилот AI traffic самолета был найден,
    // то переставляем его, управлять самолетом мы будем сами.

    if ( __dataref_autopilot ) {
        int ap = 1;
        XPLMSetDatavi( __dataref_autopilot, &ap, _aircraft_index - 1, 1 );
    } else XPlaneUtilities::log(string("ERROR: AI DataRef not found for ") + string( __AI_AUTOPILOT_PATH ));
    */

}

// *********************************************************************************************************************
// *                                                                                                                   *
// * "Управление" самолетом, пересчет позиций на основе известных старых, а так же имеющихся данных о самом самолете   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AIControlledAircraft::control(float elapsed_since_last_call) {

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Деструктор.                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AIControlledAircraft::~AIControlledAircraft() {

}
