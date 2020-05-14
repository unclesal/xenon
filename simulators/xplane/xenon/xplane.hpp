// *********************************************************************************************************************
// *                                            The wrapper for XPLMUtilities                                          *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 mar 2019 at 16:19 *
// *********************************************************************************************************************

#pragma once

// System includes
#include <string>

#include "math.h"

#ifdef INTERNAL_XPLANE

// Внутри X-Plane (plugin) - подцепляем реальные исходники.
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMGraphics.h"

#else
// Для тестов. Вывод на стандартную консоль.
#include <iostream>
#endif

// Мои инклудники.
#include "structures.h"
#include "utils.hpp"

using namespace std;

namespace xenon {

    class XPlane {

        public:

            /**
             * @short Logging a message into Log.txt file.
             */

            static void log( std::string message ) {

                // Output the message. In addition we put a "our mark" and the
                // Carriage Return to this stream, to avoid broken text lines.

                std::string concated = std::string("XENON: ") + message;
#if IBM
                concated += "\r\n";
#else
                concated += "\n";
#endif

#ifdef INTERNAL_XPLANE
                // "Боевой" режим - внутри X-Plane.
                XPLMDebugString( concated.c_str() );
#else
                // "Тестовый" режим, когда кусочки плагина вызываются из других программ.
                std::cout << concated;
#endif

            };

            /**
             * @short Search opaque pointer to data structure internal of simulator for specified name.
             */

#ifdef INTERNAL_XPLANE
            /**
             * Получить ссылку на данные по определенному пути внутри X-Plane.
             */
            static XPLMDataRef find_data_ref( const char * name ) {

                XPLMDataRef result = XPLMFindDataRef( name );

                // If pointer was not found and we have an DEBUG mode, logging about it
                // for provide a invalid name correction possibility.

#ifdef DEBUG
                if ( ! result ) {
                    log(
                        std::string("XUtilites::find_data_ref(), bad variable name, DataRef not found for: ")
                        + name
                    );
                };
#endif
                return result;
            } // find_data_ref

#endif // ifdef INTERNAL_XPLANE

            /**
             * @short Получить системный путь (директорий, в котором расположен сам X-Plane).
             */

            static const string get_system_path() {

#ifdef INTERNAL_XPLANE
                char system_path[1024];
                XPLMGetSystemPath( (char *) system_path);
                return string(system_path);
#else
                return string("/home/sal/program/X-Plane 11");
#endif

            }


            /**
             * Получить разделитель директориев, обеспечение кросс-платформенности.
             */

            static const string get_directory_separator() {
#ifdef INTERNAL_XPLANE
                // Реальное выполнение плагина внутри X-Plane.
                return string(XPLMGetDirectorySeparator());
#else
                // Мое собственное тестирование - явно же под Linux?
                return string("/");
#endif
            }

            /**
             * Путь к корню плагина, последним идет сепаратор.
             */
            static string get_plugin_path() {
#ifdef INTERNAL_XPLANE
                // Реальный запуск, внутри плагина.
                string system_path = XPlane::get_system_path();
                string separator = XPlane::get_directory_separator();
                // Системный путь X-Plane в оригинале заканчивается - сепаратором.
                string xenon_plugin_dir = system_path + "Resources" + separator
                    + "plugins" + separator + "xenon" + separator;
                return xenon_plugin_dir;
#else
                // Отладочный режим, не внутри плагина.
                return "/home/sal/program/X-Plane 11/Resources/plugins/xenon/";
#endif
            }

            /**
             * @short Перевод координат из 3D OGL игровых - в гео-формат.
             * @param position
             * @return
             */
            static location_t position_to_location( const position_t & position ) {
#ifdef INTERNAL_XPLANE
                location_t location;
                XPLMLocalToWorld(
                    position.x, position.y, position.z,
                    & location.latitude, & location.longitude, & location.altitude
                );
                return location;
#else
                return location_t();
#endif
            }

            /**
             * @short Перевод координат из гео-формата в локальную 3D OGL форму.
             * @param location
             * @return
             */
            static position_t location_to_position( const location_t & location ) {

#ifdef INTERNAL_XPLANE
                position_t position;
                XPLMWorldToLocal(
                    location.latitude, location.longitude, location.altitude,
                    & position.x, & position.y, &position.z
                );
                return position;
#else
                return position_t();
#endif
            }

            /**
             * @short Курс (азимут) от точки на точку.
             */

            static double bearing(const location_t & location_from, const location_t & location_to ) {
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
                // brng = ( ((int)brng + 360) % 360 );
                return brng;
            }

            static double bearing( const position_t & position_from, const position_t & position_to ) {
                auto location_from = position_to_location( position_from );
                auto location_to = position_to_location( position_to );
                return bearing( location_from, location_to );
            }

            /**
             * @short Дистанция между двумя точками - в "плоском" ее смысле, без учета высоты.
             */
            static double distance_2d(const position_t & p1, const position_t & p2 ) {
                return sqrt( pow(( p1.x - p2.x ), 2) + pow(( p1.z - p2.z ), 2));
            }

            /**
             * @short Дистанция в "плоском" смысле между точкой и прямой линией.
             * https://ru.wikipedia.org/wiki/%D0%A0%D0%B0%D1%81%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%B8%D0%B5_%D0%BE%D1%82_%D1%82%D0%BE%D1%87%D0%BA%D0%B8_%D0%B4%D0%BE_%D0%BF%D1%80%D1%8F%D0%BC%D0%BE%D0%B9_%D0%BD%D0%B0_%D0%BF%D0%BB%D0%BE%D1%81%D0%BA%D0%BE%D1%81%D1%82%D0%B8
             */
            static double distance_2d( const position_t & pos, line_descriptor_t & l ) {
                // Точка, в которой пересекаются прямые l и перпендикуляр к ней, проходящий через точку pos.
                position_t pos2;
                pos2.x = ( pos.x + l.k * pos.z - l.k * l.b ) / ( pow(l.k, 2 ) + 1 );
                pos2.z = l.k * pos2.x + l.b;
                return distance_2d(pos, pos2);
            }

            /**
             * @short Полная дистанция, с учетом высоты.
             */

            static double distance_3d(const position_t & p1, const position_t & p2 ) {
                return sqrt(
                    pow(( p1.x - p2.x ), 2) + pow((p1.y - p2.y), 2) + pow((p1.z - p2.z), 2)
                );
            }

            /**
             * @short Математическое описание линии, проходящей через заданную точку и имеющей данный курс.
             */

            static line_descriptor_t line( const position_t & pos, double heading ) {
                line_descriptor_t result;
                // heading - это направление "на север". А нас интересует - "нормальный"
                // угол. Т.е. он во-первых от положительного направления оси X,
                // а во-вторых, он крутится в другую сторону, против часовой стрелки.
                // Тогда как курс увеличивается в направлении по часовой стрелке.
                double course = 360.0 - heading + 90.0;
                normalize_degrees( course );
                double radians = degrees_to_radians( course );

                result.k = tan( radians );
                result.b = pos.z - result.k * pos.x;

                return result;
            }

    };

};
