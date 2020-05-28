// *********************************************************************************************************************
// *                          Выталкивание самолета (действие, ребро графа состояний самолета)                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:27 *
// *********************************************************************************************************************

#include "aircraft_does_push_back.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesPushBack::AircraftDoesPushBack(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction( ptr_acf, edge_d )
{
    __current_phase = PHASE_NONE;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Внутренняя процедура начала действия                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::_internal_start() {
    
    _ptr_acf->is_clamped_to_ground = true;
    
    _ptr_acf->set_nav_lites( true );
    _params.tug = - TAXI_SLOW_TUG;
    _params.target_acceleration = -TAXI_SLOW_ACCELERATION;
    _params.target_speed = PUSH_BACK_SPEED;
    __current_phase = PHASE_STRAIGHT;
    
    // Угловое положение самолета надо зафиксировать на текущее.
    // Иначе он начнет поворачиваться, т.к. по умолчанию у него
    // целевой курс равен 0 градусов.
    
    _params.heading_acceleration = 0.0;
    _params.target_heading = _ptr_acf->get_rotation().heading;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *              Один "внутренний шаг" при прямолинейном выталкивании - пытаемся перейти на поворот                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::__internal_step__phase_straight() {
    // Точка, до которой нас выталкивают.
    waypoint_t wp = _get_front_wp();
    auto wp1 = _get_first_wp();

    auto wp_position = XPlane::location_to_position( wp.location );        
    
    // Описатель "целевой рулежной дорожки", куда мы собрались приехать.
    // TODO: переделать на глобальные координаты
    line_descriptor_t final_taxiway = XPlane::line( wp_position, wp.outgoing_heading );

    // "Плоское" расстояние от места положения самолета до этой самой прямой линии.
    double distance = XPlane::distance_2d( _ptr_acf->get_position(), final_taxiway );

    double d1 = xenon::distance_to_segment(
        _ptr_acf->get_location(), wp.location, wp1.location
    );
        
    // TODO: Переделать на точку соединения с дугой, а не на "лобовую" оценку расстояния.
    if ( distance <= TAXI_TURN_RADIUS ) {
        __current_phase = PHASE_TURN;
 
        double delta_heading = __get_delta_to_target_heading( wp );
        // Нормализация, потому что сейчас будем определять сторону поворота.
        normalize_degrees( delta_heading );

        // В какую сторону поворачиваем? Оно как бы несколько излишне, потому что
        // будет устанавливаться каждый раз в функции определения своего положения.
        // Но раньше его было сделать - нельзя, т.к. не был известен текущий курс.
        delta_heading < 180.0 ?
            _params.heading_acceleration = TAXI_HEADING_SHIFT_PER_SECOND 
            : _params.heading_acceleration = - TAXI_HEADING_SHIFT_PER_SECOND;
            
        // Самолету надо поставить целевой курс, чтобы он начал поворачивать.
        _params.target_heading = wp.outgoing_heading;
            
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *             Шаг в "фазе разворота" - определение того факта, что встали как-то так похоже на правду               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::__internal_step__phase_turn() {
    waypoint_t wp = _get_front_wp();
    double delta = __get_delta_to_target_heading( wp );
    normalize_degrees( delta );
    // Порог разницы в курсах, ниже которого мы считаем, что выровнялись.
    double threshold = 0.8;
    
    if ( (delta >= 360 - threshold ) || ( delta <= threshold ) ) {
        
        _params.target_heading = _ptr_acf->get_rotation().heading;
        _params.heading_acceleration = 0.0;
        
        // Останавливаемся. Знаки будут положительными, т.к. мы ехали назад.
        // Он чо-т шибко долго останавливается визуально-то. Поэтому увеличил.
        _params.tug = TAXI_SLOW_TUG * 5.0; 
        _params.target_acceleration = TAXI_SLOW_ACCELERATION * 3.0;

        // Это надо обнулить с предыдущей фазы.
        _params.acceleration = 0.0;        
        _params.target_speed = 0.0;
                
        __current_phase = PHASE_STOP;
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Фаза остановки после руления                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::__internal_step__phase_stop() {
    
    if ( abs( _params.speed ) < 0.2 ) {
        // Завершено полностью.
        _front_wp_reached();        
        _finish();        
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *          Получить разницу между текущим курсом самолета и целевым курсом на нулевой точке полетного плана         *
// *                                                                                                                   *
// *********************************************************************************************************************

double AircraftDoesPushBack::__get_delta_to_target_heading( const waypoint_t & wp ) {
    
    auto current_rotation = _ptr_acf->get_rotation();
    // Текущий курс самолета.
    double heading = current_rotation.heading;
    normalize_degrees( heading );
    // Целевой курс, как нам надо встать.
    double target_heading = wp.outgoing_heading;
    normalize_degrees( target_heading );

    double delta_heading = target_heading - heading;
    // Здесь нормализация - не выполняется. Потому что она нужна не всегда.
    return delta_heading;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Один шаг в выполнении данного действия                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::_internal_step ( const float & elapsed_since_last_call ) {
    
    switch( __current_phase ) {
        case PHASE_STRAIGHT: __internal_step__phase_straight(); break;
        case PHASE_TURN: __internal_step__phase_turn(); break;
        case PHASE_STOP: __internal_step__phase_stop(); break;
        default: break;
    };    
}



