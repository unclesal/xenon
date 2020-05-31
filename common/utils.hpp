// *********************************************************************************************************************
// *                                               Общие мелкие утилитки.                                              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:32 *
// *********************************************************************************************************************
#pragma once

#include <sys/time.h>

#include <math.h>

#include <string>
#include <array>
#include <vector>
#include "constants.h"
#include "settings.h"
#include "structures.h"

using namespace std;
using namespace xenon;

#ifndef INSIDE_XPLANE
#include <GeographicLib/Geodesic.hpp>
using namespace GeographicLib;
#endif

namespace xenon {

    /**
     * @short Разбиение строки на массив подстрок по указанному разделителю.
     */
    inline vector<std::string> split (const string & s, char delim) {
        vector<string> result;
        stringstream ss (s);
        string item;

        while (getline (ss, item, delim)) {
            result.push_back (item);
        }

        return result;
    };    

    /**
     * @short Нормализация градусов.
     * Курс (азимут) может быть в пределах 0-360.
     */

    inline void normalize_degrees( double & degrees ) {

        while ( degrees > 360.0 ) degrees -= 360.0;
        while ( degrees < 0.0 ) degrees += 360.0;

    }

    /**
     * @short Перевод из радиан в градусы.
     * @param radians
     * @return
     */

    inline double radians_to_degrees( const double & radians ) {
        double degrees = radians * 180.0 / PI;
        normalize_degrees( degrees );
        return degrees;
    };

    /**
     * @short Перевод из градусов в радианы.
     * @param degrees
     * @return
     */

    inline double degrees_to_radians( const double & degrees ) {
        return degrees * PI / 180.0;
    };
    
    inline double meters_per_second_to_knots( const double & mps ) {
        // 1 м/с = 1.9438444924574 kt
        // return 3600.0 * mps / (double) M_per_NM;
        return mps * 1.9438444924574;            
    };

    inline double knots_to_merets_per_second( const double & kt ) {
        // 1 kt = 0.51444444444 м/с
        return 0.51444444444 * kt;
    };
    
    inline double meters_per_seconds_to_feet_per_min( const double mps ) {
        // 1 фут в минуту = 0.00507999983744 метра в секунду.
        return mps / 0.00507999983744;
    };
    
    inline double feet_per_min_to_meters_per_second( const double fpm ) {
        return fpm * 0.00507999983744;
    };

    /**
     * @short Футы в метры.
     */
    inline double feet_to_meters( const double & feet ) {
        // 1 ft = 0,3048 м
        return feet * 0.3048;
    };

    /**
     * @short Полная дистанция в сферических координатах.
     */

//    inline double distance3d( const location_t & from, const location_t & to ) {
//        auto boost_from = boost_location_t( from.latitude, from.longitude, from.altitude );
//        auto boost_to = boost_location_t( to.latitude, to.longitude, to.altitude );
//        double distance = boost::geometry::distance( boost_from, boost_to, geoid_distance_t());
//        // Он похоже не в метрах считает-то.
//        return distance;
//    };

    inline double distance2d( const location_t & from, const location_t & to ) {

        // Формула "гаверсинуса"

        double lat1 = degrees_to_radians( from.latitude );
        double lon1 = degrees_to_radians( from.longitude );

        double lat2 = degrees_to_radians( to.latitude );
        double lon2 = degrees_to_radians( to.longitude );

        double sin1 = sin(( lat1 - lat2 ) / 2.0 );
        double sin2 = sin(( lon1 - lon2 ) / 2.0 );

        return 2.0 * EARTH_RADIUS *asin(sqrt(sin1*sin1+sin2*sin2*cos(lat1)*cos(lat2)));
    };
    
    /**
     * @short Курс (азимут) от точки на точку.
     */

    inline double bearing(const location_t & location_from, const location_t & location_to ) {
        double teta1 = degrees_to_radians( location_from.latitude ); // lat
        double teta2 = degrees_to_radians( location_to.latitude ); // lat2
        double delta1 = degrees_to_radians( location_to.latitude - location_from.latitude ); // lat2-lat
        double delta2 = degrees_to_radians( location_to.longitude - location_from.longitude ); // lon2-lon

        //==================Heading Formula Calculation================//

        double y = sin(delta2) * cos(teta2);
        double x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);
        double brng = atan2(y,x);
        brng = radians_to_degrees(brng);// radians to degrees
        normalize_degrees( brng );
        return brng;
    };

#ifndef INSIDE_XPLANE
    inline location_t shift( const location_t & from, const double & meters, const float & heading ) {

        const Geodesic & geod = GeographicLib::Geodesic::WGS84();

        location_t to;
        to.altitude = from.altitude;

        double azi2 = 0.0, m12 = 0.0, M12 = 0.0, M21 = 0.0, S12 = 0.0;

        geod.Direct(
            from.latitude, from.longitude, heading, meters,
            to.latitude, to.longitude, azi2, m12, M12, M21, S12
        );

        return to;
    };
#endif

    /**
     * @brief Расстояние от точки до линии, определенной двумя другими точками.
     * @param from_point Точка, от которой хотели бы знать расстояние.
     * @param segment_point_1 Точка "начала линии".
     * @param segment_point_2 Точка "конца линии".
     * @return
     */

    inline double distance_to_segment (
            const location_t & from_point, const location_t & segment_point_1, const location_t & segment_point_2
    ) {

        double d0 = distance2d( from_point, segment_point_1 );
        double d1 = distance2d( segment_point_1, segment_point_2);
        double d2 = distance2d( from_point, segment_point_2 );

        double half_perimeter = (d0 + d1 + d2) * 0.5;
        double area = sqrt(half_perimeter * ( half_perimeter - d0 ) * ( half_perimeter - d1 ) * ( half_perimeter - d2 ));

        double distance_to_line = 2.0 * area / distance2d ( segment_point_1, segment_point_2 );
        return distance_to_line;

    };
    
    /**
     * @short Преобразование градусов из минут-секунд - в десятичную форму.
     */
    
    inline double degrees_to_decimal( const double & grad, const double & min, const double & sec, const char & dir ) {
        double result = grad + min / 60.0 + sec / 3600.0;
        if ( ( dir == 'S' ) || ( dir == 'W' ) ) result = -result;
        return result;
    };
    
    /**
     * @short Вернуть сервер, на котором расположен коммуникатор.
     */
    
    inline std::string get_communicator_host() {
        return COMMUNICATOR_HOST;
    };
    
    /**
     * @short Вернуть порт коммуникатора.
     */
    inline int get_communicator_port() {
        return COMMUNICATOR_PORT;
    };
    
    /**
     * @short Вернуть максимальное число соединений, допустимых для коммуникатора.
     */
    
    inline int get_communicator_max_clients() {
        return COMMUNICATOR_MAX_CLIENTS;
    };
    
    inline long int get_system_time_ms() {
        struct timeval tp;
        gettimeofday( & tp, nullptr );
        long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
        return ms;
    };

}; // namespace xenon
