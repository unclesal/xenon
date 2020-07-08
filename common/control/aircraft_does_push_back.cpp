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
    
    _ptr_acf->vcl_condition.is_clamped_to_ground = true;
    
    _ptr_acf->set_nav_lites( true );
    _ptr_acf->vcl_condition.acceleration = PUSH_BACK_SPEED / 15.0f;
    
    _ptr_acf->vcl_condition.target_speed = PUSH_BACK_SPEED;
    __current_phase = PHASE_STRAIGHT;
    
    // Угловое положение самолета надо зафиксировать на текущее.
    // Иначе он начнет поворачиваться, т.к. по умолчанию у него
    // целевой курс равен 0 градусов.
    
    _ptr_acf->vcl_condition.heading_acceleration = 0.0;
    _ptr_acf->vcl_condition.target_heading = _ptr_acf->get_rotation().heading;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *              Один "внутренний шаг" при прямолинейном выталкивании - пытаемся перейти на поворот                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::__internal_step__phase_straight() {
    
    waypoint_t wp = _ptr_acf->front_waypoint();
    if ( _taxi_turn_started(wp) ) __current_phase = PHASE_TURN;
    

    /*
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
    */

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *             Шаг в "фазе разворота" - определение того факта, что встали как-то так похоже на правду               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::__internal_step__phase_turn() {
    waypoint_t wp = _ptr_acf->front_waypoint();
    double delta = _get_delta_to_target_heading( wp );
    normalize_degrees( delta );
    // Порог разницы в курсах, ниже которого мы считаем, что выровнялись.
    double threshold = 0.8;
    
    if ( (delta >= 360 - threshold ) || ( delta <= threshold ) ) {
        
        // Останавливаемся.
        _ptr_acf->vcl_condition.target_heading = _ptr_acf->get_rotation().heading;
        _ptr_acf->vcl_condition.heading_acceleration = 0.0;        
        _taxi_breaking( 0.0, 5.0 );
                
        __current_phase = PHASE_STOP;
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Фаза остановки после руления                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::__internal_step__phase_stop() {
    
    if ( abs( _ptr_acf->vcl_condition.speed ) < 0.2 ) {
        // Завершено полностью.
        Logger::log("does_push_back finished");
        _front_wp_reached();
        Logger::log("front_wp_reached was called, call finish...");
        _finish();        
    }

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



