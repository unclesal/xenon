// *********************************************************************************************************************
// *                                                    Аэропорт.                                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 13:22 *
// *********************************************************************************************************************

#include <string>
#include <clocale>
#include <iostream>
#include <fstream>
#include <filesystem>

#ifdef INSIDE_XPLANE
#include "XPLMNavigation.h"
#endif

#include "structures.h"
#include "utils.hpp"
#include "airport.h"
#include "exceptions.h"
#include "airport_network.h"

namespace fs = std::filesystem;
using namespace std;
using namespace lemon;
using namespace xenon;

// Статические найденные аэропорты.
map<string, Airport> Airport::__airports;
bool Airport::__airports_was_readed = false;

// Обработчик события чтения нового аэропорта.
void ( * Airport::__has_been_parsed )( string & icao_code ) = nullptr;
// Фейковый аэропорт.
Airport Airport::__fake_airport;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Пустой конструктор.                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

Airport::Airport() {
    __full_path_to_apt_dat = "";
    __origin = "";
    __version = 0;
    __ap_type = AP_TYPE_UNKNOWN;
    __evalution_in_feet = 0;
    __icao_code = "";
    __name = "";
    __viewpoint = viewpoint_t();

    // Атрибуты аэропорта.
    __city = "";
    __country = "";
    __datum_lon = 0.0;
    __datum_lat = 0.0;
    __gui_label = "";
    __iata_code = "";
    __region_code = "";
    __state = "";
    __transition_alt = "";
    __transition_level = "";
    __faa_code = "";
    __flatten = 0;
    __drive_on_left = false;
    __local_code = "";

    // Объекты внутри аэропорта.

    __land_runways = vector< land_runway_t >();
    __water_runways = vector< water_runway_t >();
    __helipad_runways = vector< helipad_runway_t >();
    __startup_locations = map< string, startup_location_t >();
    __truck_parkings = vector< truck_parking_t >();
    __truck_destinations = vector< truck_destination_t >();

    // Объекты-контейнеры нодов внутри аэропорта.
    __light_beacons = vector< light_beacon_t >();
    __frequencies = vector< frequency_t >();
    __windsocks = vector< windsock_t >();
    __signs = vector< sign_t >();
    __lighting_objects = vector< lighting_objects_t >();
    __taxiways = vector< taxiway_t >();
    __linear_features = vector< linear_feature_t >();
    __boundaries = vector< boundary_t >();

    // traffic rules flow
    __traffic_flow = vector< traffic_flow_t >();

    // "Осиротевшесть" аэропорта.
    __orphanded = true;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Парзим - один файл.                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::read( const string & full_path_to_apt_dat ) {

    if ( ! std::filesystem::exists(full_path_to_apt_dat)) return;

    string origin;
    int version = 0;
    // Динамически создаваемые указатели на элементы, которые могут иметь продолжение в файле.
    // А могут и не иметь. Поэтому наличие в них какого-то значения надо учитывать при выходе из процедуры.
    Airport * apt = nullptr;
    node_container_t * node_container = nullptr;
    startup_location_t * startup_location = nullptr;
    traffic_flow_t * traffic_flow = nullptr;

    ifstream file( full_path_to_apt_dat );

//    std::locale locale("C");
//    file.imbue(locale);
//
//    char point = std::use_facet< std::numpunct<char> >(file.getloc()).decimal_point();
//    XPlaneUtilities::log( string("Delimiter is: ") + point );

    int lines_count = 0;
    for( std::string line; getline( file, line ); ) {
        // Счетчик числа строк, для отладки, если в файле будут найдены
        // ошибки или данный файл будет некорректно обработан.
        lines_count ++;

//        if (
//            (full_path_to_apt_dat == "/home/sal/program/X-Plane 11/Custom Scenery/RescueX_Lib/Earth nav data/apt.dat")
//            && (lines_count == 87)
//        ) {
//            cout << full_path_to_apt_dat << ", " << lines_count << endl;
//        }

        // Удаление из строки символов возврата каретки и перевода строки.
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());

        // Если до сих пор не заполнен Origin, то там допускается только два варианта.
        if ( origin.empty() ) {
            if (( line == "I" ) || ( line == "A" )) {
                origin = line;
                continue;
            } else throw bad_format_exception( string("Bad origin line ") + line );
        }

        // Разбиваем строку на подстроки, разделителем является пробел.
        vector<string> contents = split(line, ' ');
        // Удаляем пустые элементы полученного массива.
        contents.erase(
            remove_if(contents.begin(), contents.end(), [](string const & s) { return s.empty(); }),
            contents.end()
        );

        if ( contents.empty() ) continue;
        // В некоторых попадается решетка перед типом.
        int pos = contents[0].find('#');
        if ( pos >= 0 ) contents[0].erase(pos, 1);
        int i_type = stoi(contents.at(0));

        // Если до сих пор нет версии - то пытаемся ее заполнить.

        if ( ! version ) {
            if ( i_type >= 850 ) { // Меньше я ее вроде не встречал.
                version = i_type;
                continue;
            }
        }

//        if (
//            (apt) && ( apt->_icao_code_ == "USSS" ) && ( i_type == 1300 ) && (line.find(" 13") > 0)
//            && ( line.find("heavy") >0 )
//        ) {
//            cout << "Here";
//        };

        // -------------------------------------------------------------------------------------------------------------
        //
        //                                   Все. Дальше пошли элементы аэропорта.
        //
        // -------------------------------------------------------------------------------------------------------------

        if (( i_type == AP_TYPE_LAND ) || ( i_type == AP_TYPE_SEA ) || ( i_type == AP_TYPE_HELI )) {
            // Начало аэропорта.
            if ( apt ) {
                // Если у нас уже что-то было насчитано, то аэропорт надо поместить в массив результата.
                // Потому что начался следующий. При этом если элемент уже есть, то надо попытаться
                // сохранить его атрибуты (1302 которые).
                if ( startup_location ) apt->__put_startup_location( & startup_location );
                if ( node_container ) apt->__put_node_container( & node_container );
                if ( traffic_flow ) apt->__put_traffic_flow( & traffic_flow );
                Airport::__put_airport( & apt );
            }
            apt = new Airport();
            // Начальное заполнение того, что уже должно быть - элементы
            // одни и те же для всех аэропортов данного файла (поскольку
            // в одном файле может быть - несколько аэропортов).
            apt->__origin = origin;
            apt->__version = version;
            apt->__full_path_to_apt_dat = full_path_to_apt_dat;
            // Здесь же - элементы этой самой начальной строки.
            apt->__ap_type = i_type;
            apt->__evalution_in_feet = stoi(contents.at(1));
            apt->__icao_code = contents.at(4);
            pos = line.find( apt->__icao_code );
            pos += apt->__icao_code.length();
            apt->__name = line.substr(pos);
            while (apt->__name.at(0) == ' ') apt->__name.erase( 0, 1 );
            continue;
        }

        if ( i_type == 14 ) {
            // viewpoint
            if ( ! apt ) throw runtime_error("got viewpoint while apt is none. Line=" + to_string(lines_count));
            viewpoint_t vp;
            vp.latitude = stod( contents[1] );
            vp.longitude = stod( contents[2] );
            vp.height = stoi( contents[3] );
            // 4 - not used yet.
            // С 5ой позиции начиная и там их может быть много с пробелами.
            pos = line.find(contents[5]);
            vp.name = line.substr( pos );
            apt->__viewpoint = vp;
            continue;
        }

        if ( i_type == 15 ) {
            // startup location - видимо, в старом стиле, дублирует 1300.
            if ( ! apt ) throw runtime_error(
                    "got startup location but apt is empty. line=" + to_string(lines_count)
                );
            if ( startup_location ) apt->__put_startup_location( & startup_location );
            startup_location = new startup_location_t();

            // Структура локации, содержащая высоту. Иначе у нас
            // самолеты вне X-Plane будут ползать под землей.
            startup_location->location.latitude = stod( contents[1] );
            startup_location->location.longitude = stod( contents[2] );
            startup_location->location.altitude = apt->evalution_in_meters();

            startup_location->heading = stof( contents[3] );
            // Имя - это вся строка до конца.
            pos = line.rfind( contents[4] );
            startup_location->name = line.substr( pos );
            continue;

        }

        if ( i_type == 18 ) {
            // Light beacon
            if ( ! apt ) throw runtime_error("got light beacon while apt is empty, line=" + to_string(lines_count));
            light_beacon_t lb;
            lb.latitude = stod( contents[1] );
            lb.longitude = stod( contents[2] );
            lb.type_code = stoi( contents[3] );
            if ( contents.size() >= 5 ) {
                pos = line.find( contents[4] );
                lb.name = line.substr( pos );
            }
            apt->__light_beacons.push_back(lb);
            continue;
        }

        if ( i_type == 19 ) {
            // windsock.
            if ( ! apt ) throw runtime_error("Got a windsock while apt is empty, line=" + to_string(lines_count));
            windsock_t ws;
            ws.latitude = stod( contents[1] );
            ws.longitude = stod( contents[2] );
            ws.flag_lighting = stoi( contents[3] );
            if ( contents.size() >= 5 ) {
                pos = line.find(contents[4]);
                ws.name = line.substr(pos);
            }
            apt->__windsocks.push_back( ws );
            continue;
        }

        if ( i_type == 20 ) {
            // Sign
            if ( ! apt ) throw runtime_error("got sign while apt is none. Line=" + to_string(lines_count));
            sign_t sign;
            sign.latitude = stod( contents[1] );
            sign.longitude = stod( contents[2] );
            sign.orientation = stof( contents[3] );
            // 4 - в данный момент не используется.
            sign.size_code = stoi( contents[5] );
            sign.label = contents[6];
            apt->__signs.push_back(sign);
            continue;
        }

        if ( i_type == 21 ) {
            // Lighting object.
            if ( ! apt ) throw runtime_error("got lighting object, apt is none. line=" + to_string(lines_count));
            lighting_objects_t lo;
            lo.latitude = stod( contents[1] );
            lo.longitude = stod( contents[2] );
            lo.object_type = stoi( contents[3] );
            lo.orientation = stof( contents[4] );
            lo.glidescope_angle = stof( contents[5] );
            lo.runway_number = contents[6];
            // description is optional.
            if ( contents.size() >= 8 ) lo.description = contents[7];
            apt->__lighting_objects.push_back( lo );
            continue;
        }

        if (
            ( i_type >= FREQ_OLD_ATC_RECORDED && i_type <= FREQ_OLD_ATC_DEP )
            || ( i_type >= FREQ_ATC_RECORDED && i_type <= FREQ_ATC_DEP )
        ) {
            // Частоты. Коды начинаются от 50.
            if ( ! apt ) throw runtime_error("Got frequency but apt is empty, line=" + to_string(lines_count) );
            frequency_t freq;
            freq.freq_type = i_type;
            freq.frequency = stoi( contents[1] );
            if ( contents.size() >= 3 ) {
                pos = line.find(contents[2]);
                freq.description = line.substr(pos);
            }
            apt->__frequencies.push_back( freq );
            continue;
        }

        if ( i_type == 100 ) {
            // LAND RUNWAYs.
            if ( ! apt ) throw runtime_error(
                "got landing RWY while apt is none, line=" + to_string(lines_count)
            );

            land_runway_t rwy;
            rwy.width = stof( contents[1] );
            rwy.surface_type = stoi( contents[2] );
            rwy.shoulder_surface_type = stoi( contents[3] );
            rwy.smoothness = stof( contents[4] );
            rwy.centre_line_light = stoi( contents[5] );
            rwy.edge_light = stoi( contents[6] );
            rwy.distance_remaining_sign = stoi( contents[7] );
            // Дальше атрибутов может уже не быть.
            if ( contents.size() >= 9 ) rwy.runway_number = contents[8];
            if ( contents.size() >= 10 ) rwy.end_latitude = stod( contents[9] );
            if ( contents.size() >= 11 ) rwy.end_longitude = stod( contents[10] );
            if ( contents.size() >= 12 ) rwy.displaced_threshold_length = stof( contents[11] );
            if ( contents.size() >= 13 ) rwy.overrun_length = stof( contents[12] );
            if ( contents.size() >= 14 ) rwy.markings = stoi( contents[13] );
            if ( contents.size() >= 15 ) rwy.approach_ligth = stoi( contents[14] );
            if ( contents.size() >= 16 ) rwy.tdz_ligth = stoi( contents[15] );
            if ( contents.size() >= 17 ) rwy.reil_light = stoi( contents[16] );
            apt->__land_runways.push_back( rwy );
            continue;
        }

        if ( i_type == 101 ) {
            // WATER RUNWAYs.
            if ( ! apt ) throw runtime_error(
                "got water RWY while apt is none, line=" + to_string(lines_count)
            );

            water_runway_t rwy;
            rwy.width = stof( contents[1] );
            rwy.perimeter_buoys = stoi( contents[2] );
            // Дальше атрибутов может уже не быть.
            if ( contents.size() >= 4 ) rwy.runway_number = contents[3];
            if ( contents.size() >= 5 ) rwy.latitude = stod( contents[4] );
            if ( contents.size() >= 6 ) rwy.longitude = stod( contents[5] );
            apt->__water_runways.push_back( rwy );
            continue;
        }

        if ( i_type == 102 ) {
            // HELIPAD RUNWAYs
            if ( ! apt ) throw runtime_error(
                "got helipad RWY while apt is none, line=" + to_string( lines_count )
            );

            helipad_runway_t rwy;
            rwy.designator = contents[1];
            rwy.latitude = stod( contents[2] );
            rwy.longitude = stod( contents[3] );
            rwy.orientation = stof( contents[4] );
            rwy.length = stof( contents[5] );
            rwy.width = stof( contents[6] );
            rwy.surface_code = stoi( contents[7] );
            rwy.markings = stoi( contents[8] );
            rwy.surface_type = stoi( contents[9] );
            if (contents.size() >= 11 ) {
                rwy.smoothness = stof( contents[ 10 ] );
                if ( contents.size() >= 12 ) rwy.edge_light = stoi( contents[ 11 ] );
            }
            apt->__helipad_runways.push_back( rwy );
            continue;
        }

        if ( i_type == CONTAINER_TAXIWAY ) { // 110
            // pavement (taxiway).
            if ( ! apt ) throw runtime_error(
                "got taxiway while apt is none, line=" + to_string( lines_count )
            );
            if ( node_container ) apt->__put_node_container( & node_container );
            node_container = new taxiway_t();
            auto * twy = ( taxiway_t * ) node_container;
            twy->container_type = CONTAINER_TAXIWAY;
            twy->surface_type = stoi( contents[1] );
            twy->smoothness = stof( contents[2] );
            twy->orientation = stof( contents[3] );
            if ( contents.size() >= 5 ) {
                pos = line.find(contents[4]);
                twy->description = line.substr( pos );
            }

            continue;
        }

        if ( i_type == CONTAINER_LINEAR_FEATURE ) {
            if ( ! apt ) throw runtime_error("got linear feature while apt is none, line=" + to_string(lines_count));
            if ( node_container ) apt->__put_node_container( & node_container );
            node_container = new linear_feature_t();
            auto * lft = ( linear_feature_t * ) node_container;
            lft->container_type = CONTAINER_LINEAR_FEATURE;
            lft->name = line.substr(3);
            // Удаление начальных пробелов.
            while (lft->name[0] == ' ') lft->name.erase(0, 1);
            continue;
        }

        if ( i_type == CONTAINER_BOUNDARY ) {
            if ( ! apt ) throw runtime_error(
                "got boundary element when apt is none, line=" + to_string( lines_count )
            );
            if ( node_container ) apt->__put_node_container( & node_container );
            node_container = new boundary_t();
            auto * bdy = ( boundary_t * ) node_container;
            bdy->container_type = CONTAINER_BOUNDARY;
            bdy->description = contents[1];
            continue;
        }

        if ( ( i_type == NODE_PLAIN ) || ( i_type == NODE_BEZIER )
            || ( i_type == NODE_CLOSE_BOUNDARY) || ( i_type == NODE_CLOSE_BEZIER )
            || ( i_type == NODE_END_LINE ) || ( i_type == NODE_END_BEZIER )
        ) {
            // Нода.
            if ( ! apt ) throw runtime_error(
                "got node while apt is empty, line " + to_string( lines_count )
            );
            if ( ! node_container ) throw runtime_error(
                "got node while container is empty, line " + to_string( lines_count )
            );
            node_t node;
            node.node_type = i_type;
            node.latitude = stod( contents[1] );
            node.longitude = stod( contents[2] );
            int next_elem = 3;
            if (( i_type == NODE_BEZIER ) || ( i_type == NODE_CLOSE_BEZIER ) || ( i_type == NODE_END_BEZIER )) {
                node.bezier_point_latitude = stod( contents[ next_elem ++ ] );
                node.bezier_point_longitude = stod( contents[ next_elem ++ ] );
            }
            if ( ((int) contents.size()) >= next_elem + 2) {
                node.line_type_code = stoi( contents[ next_elem ++ ] );
                node.lighting_type_code = stoi( contents[next_elem] );
            }
            node_container->nodes.push_back(node);
            continue;
        }

        if ( i_type == 1000 ) {
            // traffic_flow
            if ( ! apt ) throw runtime_error("Got traffic flow while apt is empty, line=" + to_string( lines_count ));
            if ( traffic_flow ) apt->__put_traffic_flow( & traffic_flow );
            traffic_flow = new traffic_flow_t();
            pos = line.find( contents[1] );
            traffic_flow->name = line.substr( pos );
            continue;
        }

        if ( i_type == 1001 ) {
            // Traffic flow wind rule
            if ( ! apt ) throw runtime_error("Got rule wind while apt is empty, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got rule wind while traffic flow is empty, line=" + to_string( lines_count )
            );
            traffic_flow_wind_rule_t tw;
            tw.metar_station = contents[1];
            tw.wind_direction_minimum = stoi( contents[2] );
            tw.wind_direction_maximum = stoi( contents[3] );
            tw.wind_speed_maximum = stoi( contents[4] );
            traffic_flow->wind_rules.push_back( tw );
            continue;
        }

        if ( i_type == 1002 ) {
            // Traffic flow ceiling rule
            if ( ! apt ) throw runtime_error(
                "Got fraffic ceiling rule while apt is empty, line=" + to_string( lines_count )
            );
            if ( ! traffic_flow ) throw runtime_error(
                "Got ceiling rule while traffic flow is empty, line=" + to_string( lines_count )
            );
            traffic_flow_ceiling_rule_t cr;
            cr.metar_station = contents[1];
            cr.minimum_height = stoi( contents[2] );
            traffic_flow->ceiling_rules.push_back( cr );
            continue;
        }

        if ( i_type == 1003 ) {
            // Traffic flow visibility rule
            if ( ! apt ) throw runtime_error("Got visibility rule but apt is emtpy, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got visibility rule while traffic_flow is emtpy, line=" + to_string( lines_count )
            );
            traffic_flow_visibility_rule_t vis;
            vis.metar_station = contents[1];
            vis.minimum_visibility = stof( contents[2] );
            traffic_flow->visibility_rules.push_back( vis );
            continue;
        }

        if ( i_type == 1004 ) {
            // Traffic time rule
            if ( ! apt ) throw runtime_error("Got time rule, apt is empty, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got time rule, traffic flow empty, line=" + to_string( lines_count )
            );

            // Эта ситуация - бывает. Пока не знаю, что с ней делать.
            // if (( ! traffic_flow->time_from.empty() ) || ( ! traffic_flow->time_to.empty() ))
            //    throw runtime_error("got traffic time rule with already timed, line=" + to_string( lines_count ));

            traffic_flow->time_from = contents[1];
            traffic_flow->time_to = contents[2];
            continue;
        }

        if ( i_type == 1100 || i_type == 1110 ) {
            // runway in use
            if ( ! apt ) throw runtime_error("Got runway in use, but apt is empty, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got runway in use, but traffic flow is empty, line=" + to_string( lines_count )
            );
            traffic_flow_runway_in_use_t riu;
            riu.runway = contents[1];
            riu.frequency = stoi( contents[2] );
            riu.rule_type = contents[3];
            riu.airplane_types = contents[4];
            riu.on_course_heading = contents[5];
            riu.atc_heading = contents[6];
            if ( contents.size() >= 8 ) {
                pos = line.find( contents[ 7 ] );
                riu.name = line.substr( pos );
            }
            traffic_flow->runway_in_use.push_back( riu );
            continue;
        }

        if ( i_type == 1101 ) {
            // VFR pattern rule
            if ( ! apt ) throw runtime_error("Got VFR pattern but apt is empty, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got VFR pattern but traffic flow is empty, line=" + to_string(lines_count)
            );
            string rwy = contents[1];
            string direction = contents[2];
            traffic_flow->vfr_pattern[rwy] = direction;
            continue;
        }
        if ( i_type == 1200 ) {
            // taxi network
            if ( ! apt ) throw runtime_error("got taxi network while apt is empty, line=" + to_string( lines_count ));
            continue;
        }

        if ( i_type == 1201 ) {
            // taxi routing NODE.
            if ( ! apt ) throw runtime_error("got taxi routing node but apt is emtpy, line=" + to_string( lines_count ));
            apt->__routes.add_apt_dat_node( line, contents );
            continue;
        }

        if (( i_type == 1202 ) || ( i_type == 1206 )) {
            // Taxi routing EDGE. Segment in taxi routing network
            if ( ! apt ) throw runtime_error("got routing edge but apt is empty, line=" + to_string( lines_count ));
            apt->__routes.add_apt_dat_edge( i_type, line, contents );
            continue;
        }

        if ( i_type == 1204 ) {
            // Edge active zone Identifies an edge as in a runway active zone.
            if ( ! apt ) throw runtime_error("got active zone but apt is empty, line=" + to_string( lines_count ));
            apt->__routes.add_apt_dat_active_zone( line, contents );
            continue;
        }

        if ( i_type == 1300 ) {
            // startup location - видимо, в новом стиле, т.к. дублирует 15.
            if ( ! apt ) throw runtime_error(
                "got startup location but apt is empty. line=" + to_string(lines_count)
            );
            if ( startup_location ) apt->__put_startup_location( & startup_location );
            startup_location = new startup_location_t();

            // Структура локации с учетом высоты, чтобы самолеты
            // вне X-Plane не зарывались под землю.
            startup_location->location.latitude = stod( contents[1] );
            startup_location->location.longitude = stod( contents[2] );
            startup_location->location.altitude = apt->evalution_in_meters();

            startup_location->heading = stof( contents[3] );
            startup_location->startup_type = contents[4];
            startup_location->airplane_types = contents[5];
            // Наименование - вся строка до конца. Причем по последней
            // здесь ориентироваться плохо, она может быть короткой
            // и встречаться несколько раз в строке. Поэтому делаем - по предпоследней.
            if ( contents.size() >= 7 ) {
                pos = line.rfind( contents[ 6 ] );
                startup_location->name = line.substr( pos );
                while ( startup_location->name[ 0 ] == ' ' ) startup_location->name.erase( 0, 1 );
            }
            continue;
        }

        if ( i_type == 1301 ) {
            // ramp start metadata
            if ( ! apt ) throw runtime_error(
                "Got ramp start metadata but apt is empty. Line=" + to_string(lines_count)
            );
            if ( ! startup_location ) throw runtime_error(
                "Got ramp metadata but startup location is none, line=" + to_string(lines_count)
            );
            startup_location->width_code = contents[1];
            startup_location->operation_type = contents[2];
            if ( contents.size() >= 4 ) startup_location->airline = contents[3];
            continue;
        }

        if ( i_type == 1302 ) {
            // Airport identification metadata.
            if ( ! apt ) throw runtime_error(
                "got 1302 while apt is none, line=" + to_string( lines_count )
            );

            if ( contents.size() >= 3 ) {
                if ( contents[ 1 ] == "city" ) apt->__city = contents[ 2 ];
                else if ( contents[ 1 ] == "country" ) apt->__country = contents[ 2 ];
                else if ( contents[ 1 ] == "datum_lat" ) apt->__datum_lat = stod( contents[ 2 ] );
                else if ( contents[ 1 ] == "datum_lon" ) apt->__datum_lon = stod( contents[ 2 ] );
                else if ( contents[ 1 ] == "gui_label" ) apt->__gui_label = contents[ 2 ];
                else if ( contents[ 1 ] == "iata_code" ) apt->__iata_code = contents[ 2 ];
                else if ( contents[ 1 ] == "region_code" ) apt->__region_code = contents[ 2 ];
                else if ( contents[ 1 ] == "state" ) apt->__state = contents[ 2 ];
                else if ( contents[ 1 ] == "transition_alt" ) apt->__transition_alt = contents[ 2 ];
                else if ( contents[ 1 ] == "transition_level" ) apt->__transition_level = contents[ 2 ];
                else if ( contents[ 1 ] == "faa_code" ) apt->__faa_code = contents[ 2 ];
                else if ( contents[ 1 ] == "flatten" ) apt->__flatten = stoi( contents[ 2 ] );
                else if (( contents[ 1 ] == "drive_on_left" ) && ( contents[ 2 ].find( '1' ) >= 0 ))
                    apt->__drive_on_left = true;
                else if ( contents[ 1 ] == "local_code" ) apt->__local_code = contents[ 2 ];

                    // Они уже есть в основных свойствах, незачем вроде дублировать.
                else if (
                    ( contents[ 1 ] != "icao_code" ) && ( contents[ 1 ] != "airport_id" ) &&
                    ( contents[ 1 ] != "local_authority" )
                    )
                    XPlane::log( string( "Airport::read: unhandled metadata key " ) + contents[ 1 ] );
            }
            continue;
        }

        if ( i_type == 1400 ) {
            // truck parking.
            if ( ! apt ) throw runtime_error( "got truck parking but apt is empty, line=" + to_string( lines_count ));
            truck_parking_t tp;
            tp.latitude = stod( contents[1] );
            tp.longitude = stod( contents[2] );
            tp.heading = stof( contents[3] );
            tp.parking_type = contents[4];
            tp.baggage_train_type = stoi( contents[5] );
            if ( contents.size() >= 7 ) {
                pos = line.find( contents[6]);
                tp.name = line.substr( pos );
            }
            apt->__truck_parkings.push_back( tp );
            continue;
        }

        if ( i_type == 1401 ) {
            // Truck destination.
            if ( ! apt ) throw runtime_error("Got truck destination but apt is none, line=" + to_string( lines_count ));

            truck_destination_t dest;
            dest.latitude = stod( contents[1] );
            dest.longitude = stod( contents[2] );
            dest.heading = stof( contents[3] );
            dest.truck_types = contents[4];
            if ( contents.size() >= 6 ) {
                pos = line.find( contents[5] );
                dest.name = line.substr( pos );
            }
            apt->__truck_destinations.push_back( dest );

            continue;
        }

        if ( i_type != 99 ) { // EOF
            // Если здесь остались - элемент обработан не был.
            XPlane::log(
                string( "Airport::read(), unhandled type " ) + to_string( i_type )
                + ", file=" + full_path_to_apt_dat + ", line=" + to_string( lines_count )
            );
        }

    }
    // Вышли из цикла. Если кто-то из динамических
    // указателей остался живой, то надо его учесть.
    if ( apt ) {
        if ( startup_location ) apt->__put_startup_location( & startup_location );
        if ( node_container ) apt->__put_node_container( & node_container );
        if ( traffic_flow ) apt->__put_traffic_flow( & traffic_flow );
        // if ( taxi_network ) apt->_put_taxi_network( & taxi_network );
        // Сам аэропорт идет последним.
        Airport::__put_airport( & apt );
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Положить на место распарзенный аэропорт, освободить память                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::__put_airport(Airport **ptr_airport) {
    auto apt = * ptr_airport;
    string icao_code = apt->__icao_code;

    if ( Airport::__airports.contains( apt->__icao_code )) {
        // Замещение старого элемента аэропорта - более новым.
        Airport old = Airport::__airports[ apt->__icao_code ];
        if ( apt->__city.empty()) apt->__city = old.__city;
        if ( apt->__country.empty() ) apt->__country = old.__country;
        if ( apt->__datum_lon == 0.0) apt->__datum_lon = old.__datum_lon;
        if ( apt->__datum_lat == 0.0 ) apt->__datum_lat = old.__datum_lat;
        if ( apt->__gui_label.empty()) apt->__gui_label = old.__gui_label;
        if ( apt->__iata_code.empty()) apt->__iata_code = old.__iata_code;
        if ( apt->__region_code.empty()) apt->__region_code = old.__region_code;
        if ( apt->__state.empty()) apt->__state = old.__state;
        if ( apt->__transition_alt.empty() ) apt->__transition_alt = old.__transition_alt;
        if ( apt->__transition_level.empty() ) apt->__transition_level = old.__transition_level;
        if ( apt->__faa_code.empty()) apt->__faa_code = old.__faa_code;
        if ( apt->__flatten == 0 ) apt->__flatten = old.__flatten;
        if ( ! apt->__drive_on_left ) apt->__drive_on_left = old.__drive_on_left;
        if ( apt->__local_code.empty() ) apt->__local_code = old.__local_code;
    }
    // "Полнота" ВПП заполняется в тот момент, когда аэропорт складывается в коллекцию.
    apt->__check_runway_fullness();
    Airport::__airports[ apt->__icao_code ] = * apt;
    delete( apt );
    * ptr_airport = nullptr;
    // Если есть реактор на распарзенный аэропорт, то вызываем его.
    if ( Airport::__has_been_parsed ) __has_been_parsed(icao_code);
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Проверка "полноты" ВПП аэропорта. При необходимости дополняется отсутствующими ВПП.                *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::__check_runway_fullness() {

    vector<string> rwy_names = __routes.get_names_for( AirportNetwork::WAY_RUNWAY );
    for ( auto & rwy_name : rwy_names ) {

        // Заполняем полностью коллекцию узлов для составной взлетки,
        // например 08L/26R. Узлы будут одни и те же, только направление
        // движения будет обратным.
        vector <AirportNetwork::node_t> nodes = __routes.get_nodes_for( rwy_name );

        // Распарзиваем имена. Их в норме должно быть два.
        vector<string> names = split( rwy_name, '/' );
        // Все полученные "номера" (имена) ВПП должны находиться в коллекции ВПП.
        // Если их там нет, то просто ставим пока флаг. Потому что в процессе
        // данного цикла "разпознается" конец этой самой ВПП.
        string need_to_insert;
        AirportNetwork::node_t nearest_founded_node;
        land_runway_t founded_runway;

        for ( const auto & runway_number : names ) {

            bool found = false;

            for ( int i=0; i< __land_runways.size(); i++ ) {
                land_runway_t & rwy = __land_runways.at( i );
                if ( rwy.runway_number == runway_number ) {
                    // Взлетка - существует.
                    found = true;
                    founded_runway = rwy;
                    location_t location;
                    location.latitude = rwy.end_latitude;
                    location.longitude = rwy.end_longitude;
                    // Этот узел мы запомним, он будет "обратным" для добавляемой 
                    // ВПП, если такое добавление будет производиться.
                    nearest_founded_node = __routes.get_nearest_node(location, nodes);
                    
                    // Локация найденного ближайшего узла. Записываем ее, она 
                    // потом понадобиться для работы со взлетками.
                    rwy.nearest_end_location = nearest_founded_node.location;
                    
                    // Нам для взлеток нужны оба, и ближний конец, и дальний.
                    auto farest_runway_node = __routes.get_farest_node( location, nodes );
                    rwy.farest_end_location = farest_runway_node.location;
                    
                    // Выход из цикла по имеющимся RWYs.
                    break;
                }
            }

            if ( ! found ) {
                // ВПП нет. Мы все еще находимся в цикле по именам, где
                // норма - это пара ВПП. Но пока что - еще не добавление,
                // а только флаг на его необходимость.
                need_to_insert = runway_number;
            }
        }

        if (( ! need_to_insert.empty() ) && ( nearest_founded_node.xp_id >= 0 )) {
            // ВПП нужно добавить. И при этом мы действительно обнаружили какой-то
            // смысловой узел, наиболее близкий к уже существующей ВПП.
            land_runway_t rwy;
            rwy.runway_number = need_to_insert;
            location_t loc;
            loc.latitude = nearest_founded_node.location.latitude;
            loc.longitude = nearest_founded_node.location.longitude;
            AirportNetwork::node_t far_away = __routes.get_farest_node( loc, nodes );
            rwy.end_latitude = far_away.location.latitude;
            rwy.end_longitude = far_away.location.longitude;
            
            // Запоминаем координаты узлов для обоих торцов, и ближний, и дальний. Здесь они меняются местами.
            rwy.nearest_end_location = far_away.location;
            rwy.farest_end_location = nearest_founded_node.location;
            
            __land_runways.push_back( rwy );
        }

    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Положить один элемент traffic flow                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::__put_traffic_flow( traffic_flow_t ** ptr_traffic ) {
    traffic_flow_t * p_trafic = * ptr_traffic;
    if ( ! p_trafic ) return;
    __traffic_flow.push_back( * p_trafic );
    delete( p_trafic );
    * ptr_traffic = nullptr;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Положить насчитанную стоянку                                               *
// *           (стоянка может содержать - несколько строк в файле, поэтому идет через динамический указатель).         *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::__put_startup_location( startup_location_t ** ptr_startup_location ) {
    auto pst = ( startup_location_t * ) * ptr_startup_location;
    if ( ! pst ) return;
    __startup_locations[pst->name] = * pst;
    delete( pst );
    * ptr_startup_location = nullptr;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Положить ранее насчитанную taxi network                                         *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void Airport::_put_taxi_network( taxi_network_t ** ptr_taxi ) {
    auto p_taxi = * ptr_taxi;
    if ( ! p_taxi ) return;
    _taxi_networks_.push_back( * p_taxi );
    delete( p_taxi );
    * ptr_taxi = nullptr;
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Положить ранее насчитанный контейнер нодов, освободить память                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::__put_node_container( node_container_t ** ptr_container ) {
    auto container = * ptr_container;
    if ( ! container ) return;
    switch ( container->container_type ) {

        case CONTAINER_TAXIWAY: {
            auto ptwy = ( taxiway_t * ) container;
            __taxiways.push_back( * ptwy );
        } break;

        case CONTAINER_LINEAR_FEATURE: {
            auto lft = ( linear_feature_t * ) container;
            __linear_features.push_back( * lft );
        } break;

        case CONTAINER_BOUNDARY: {
            auto bdy = ( boundary_t * ) container;
            __boundaries.push_back( * bdy );
        } break;

        default: throw runtime_error(
            "put node to container, unhandled type " + to_string(container->container_type)
        );
    }
    delete container;
    * ptr_container = nullptr;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Вернуть ближайший (-е) аэропорт (-ы)                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::get_nearest( location_t & location ) {
#ifdef INSIDE_XPLANE
#endif
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Чтение и парзинг всех имеющихся сценариев.                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::read_all() {
    // Без нее идут ошибки преобразования строк в числа.
    setlocale(LC_ALL, "C");

    string system_path = XPlane::get_system_path();
    string sep = XPlane::get_directory_separator();

    // Вектор полных путей к найденным файлам аэропортов.
    vector<string> founded;

    // Для отладки - один Кольцово.
    founded.push_back(
        system_path + sep + "Custom Scenery" + sep + "USSS_Koltsovo_2012" + sep + "Earth nav data" + sep + "apt.dat"
    );

//    // Аэропорты по умолчанию.
//    founded.push_back( system_path + sep + "Resources" + sep + "default scenery"
//        + sep + "default apt dat" + sep + "Earth nav data" + sep + "apt.dat"
//    );
//    founded.push_back( system_path + sep + "Custom Scenery" + sep + "Global Airports"
//        + sep + "Earth nav data" + sep + "apt.dat"
//    );
//
//    for (const auto & entry : fs::recursive_directory_iterator(system_path)) {
//        string file_name = entry.path().filename().string();
//        if ( file_name == "apt.dat" ) {
//            string entry_full_path = entry.path().string();
//            // Добавление пути происходит только в том случае, если его еще нет.
//            // Таким образом, аэропорты по умолчанию будут затерты теми, которые
//            // есть в Custom Scenery.
//            bool exists = false;
//            // Пробовал через find, почему-то не получил нужного мне результата. Переделал "в лоб".
//            for (const auto & elem: founded) {
//                if (elem == entry_full_path) {
//                    exists = true;
//                    break;
//                }
//            }
//
//            if ( ! exists ) {
//                founded.push_back( entry_full_path );
//            }
//        }
//    }

    // Пути - нашли. Теперь парзим каждый из этих аэропортов и
    // добавляем его в коллекцию. Возможно, замещая при этом.
    for (auto & path : founded) {
        try {
            Airport::read( path );
        } catch ( bad_format_exception & e ) {
            XPlane::log( "Airport parse file " + path + ": " + e.what() );
        } catch ( range_error & e ) {
            XPlane::log( "Airport parse file: " + path + ": " + e.what() );
        } catch ( runtime_error & e ) {
            XPlane::log( "Airport parse file " + path + ": " + e.what() );
        }
    }

    // Устанавливаем флаг того, что аэропорты были прочитаны.
    // Это - единственное место, где происходит установка данного флага.
    // Во всех остальных случаях он только читается.
    Airport::__airports_was_readed = true;
    Logger::log("Airport::read_all(): All airports (" + to_string( __airports.size() ) + ") has been readed.");

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Выдать аэропорт по ICAO                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

xenon::Airport & Airport::get_by_icao( const string & icao_code ) {
    if (( ! Airport::__airports.empty() ) && ( Airport::__airports.contains(icao_code) ))
        return Airport::__airports[icao_code];
    return Airport::__fake_airport;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Вернуть используемые ВПП с указанием их функции (departure, arrival)                       *
// *                                                                                                                   *
// *********************************************************************************************************************

vector<runway_in_use_t> Airport::get_runways_in_use() {
    vector< runway_in_use_t > result;
    // На пока что заглушка.
    runway_in_use_t r1{
        .name =  "08L",
        .used =  RUNWAY_USED_ARRIVAL
    };
    result.push_back(r1);
    r1.used = RUNWAY_USED_DEPARTURE;
    result.push_back(r1);
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                   Получить объект ВПП для проведения конкретного действия (взлета или посадки)                    *
// *                                                                                                                   *
// *********************************************************************************************************************

Airport::land_runway_t Airport::get_runway_for( const runway_used_t & use ) {

    auto rwys_in_use = get_runways_in_use();
    land_runway_t result;
    for (const auto & rwy : rwys_in_use ) {
        if (( rwy.used == use ) || ( rwy.used == RUNWAY_USED_BOTH )) {
            for ( const auto & er: __land_runways ) {
                if (er.runway_number == rwy.name) {
                    result = er;
                    break;
                }
            }
            break;
        }
    }
    return result;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *        Выдать целевую точку и целевой курс для выталкивания или выруливания самолета из его текущей позиции.      *
// *                                                                                                                   *
// *********************************************************************************************************************

deque<waypoint_t> Airport::get_taxi_way_for_departure( const location_t & from ) {

    deque<waypoint_t> result;

    // ВПП в использовании для взлета.
    land_runway_t departure_rwy = get_runway_for( RUNWAY_USED_DEPARTURE );
    if (departure_rwy.runway_number.empty()) {
        XPlane::log("Airport::get_start_location_for_departure_taxing, empty runway name for departure.");
        return result;
    }

    // Ближайший к заданному положению узел, принадлежащий именно рулежным дорожкам.
    auto nearest_node_descriptor = __routes.get_nearest_node_d( from, AirportNetwork::WAY_TAXIWAY );
    AirportNetwork::node_t nearest_node;
    try {
        nearest_node = __routes.graph()[ nearest_node_descriptor ];
    } catch ( const range_error & re ) {
        XPlane::log(
            "Airport::get_start_for_departure_taxing, no nearest node found for lat="
            + to_string(from.latitude) + ", lon=" + to_string(from.longitude)
        );
        return result;
    }

    // Кратчайший путь от найденного узла до взлетки.
    auto path = __routes.get_shortest_path( nearest_node_descriptor, departure_rwy.location() );
    for ( unsigned int i=0; i<path.size(); ++ i ) {
    // for ( auto nd: path ) {
        // Дескриптор узла.
        auto nd = path.at( i );
        // Сам узел.
        auto node_itself = __routes.graph()[nd];
        waypoint_t wp;
        wp.type = WAYPOINT_TAXING;
        wp.action_to_achieve = ACF_DOES_NORMAL_TAXING;
        wp.location = node_itself.location;
        wp.location.altitude = evalution_in_meters();
        
        // Смысла нет, там имена могут быть довольно страшными и бессмысленными.
        // wp.name = node_itself.name;
        
        result.push_back( wp );
    }
    
    // Последний узел найденного пути - это уже сама взлетка.
    result.at( result.size() - 1 ).type = WAYPOINT_RUNWAY;
    result.at( result.size() - 1 ).action_to_achieve = ACF_DOES_LINING_UP;
    
    // Предпоследней точкой поставим HP.
    
    waypoint_t hp;
    auto bearing_to_rwy = xenon::bearing( result[ result.size() - 2 ].location, result[ result.size() - 1 ].location );
    hp.location = xenon::shift( result[ result.size() - 1 ].location, -130.0, bearing_to_rwy );
    hp.location.altitude = evalution_in_meters();
    hp.name = "HP";
    hp.type = WAYPOINT_HP;
    hp.action_to_achieve = ACF_DOES_NORMAL_TAXING;
    auto position = result.end() - 1;
    result.insert(position, hp);
        
    // Самолет ничего не знает о геометрии аэропорта. Поэтому
    // ему нужно дать еще и сам взлет, т.е. "дальный торец" ВПП,
    // по отношению к которому будет осущестляться разбег.
    waypoint_t take_off_wp;

    // Последняя точка должна быть RUNWAY, иначе маршрут посчитается ошибочным.
    take_off_wp.type = WAYPOINT_RUNWAY; 
    take_off_wp.location = departure_rwy.farest_end_location;    
    take_off_wp.location.altitude = evalution_in_meters();
    take_off_wp.action_to_achieve = ACF_DOES_TAKE_OFF;
    result.push_back( take_off_wp );
    
    // Теперь проходимся поочередно по элементам,
    // выставляя курс на следующую точку и расстояние до нее.
    for ( unsigned long i=0; i<result.size() - 1; i++ ) {
        location_t l_current = result.at( i ).location;
        location_t l_next = result.at( i + 1 ).location; 
        double d = xenon::distance2d( l_current, l_next );
        // Здесь точки идут в "правильном порядке", не реверсированы.
        double bearing = xenon::bearing( l_current, l_next );
        result.at(i).distance_to_next_wp = d;
        result.at(i).outgoing_heading = bearing;        
        result.at( i + 1 ).incomming_heading = bearing;
    }
        
    return result;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Вернуть свободную парковку, подходящую для данного типа ВС                                *
// *                                                                                                                   *
// *********************************************************************************************************************

startup_location_t Airport::get_free_parking( const std::string & icao_type ) {
    startup_location_t result;
    if ( icao_type == "B738" ) result = __startup_locations["15"];
    else result = __startup_locations["10"];
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Посчитать и выдать путь для руления на парковку.                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

std::deque< waypoint_t > Airport::get_taxi_way_for_parking(
    const location_t & from, const float & heading, const startup_location_t & parking
) {
    std::deque< waypoint_t > result;

    // Ближайшая точка на ВПП к запрошенному местоположению.

    auto nearest_d = __routes.get_nearest_node_d( from, AirportNetwork::WAY_RUNWAY );
    if ( nearest_d == AirportNetwork::graph_t::null_vertex()) {
        Logger::log("ERROR: Airport::get_taxi_way_for_parking(), nearest waypoint on RUNWAY not found.");
        return result;
    }    

    try {
        // Сам нод найденной точки. Причем, нужен тот, который 
        // от нас спереди, по ВПП самолеты задом не ездят.
        
        bool node_found = false;
        auto nearest_node = __routes.graph()[ nearest_d ];
        auto bearing = xenon::bearing(from, nearest_node.location);
        auto dh = bearing - heading;
        if ( abs(dh) <= 30.0 ) node_found = true;
        else {
            
            // Узел найти не удалось. Он может и "ближайший", но проблема
            // в том, что он - за нами, на не впереди нас.
            
            auto edges = __routes.get_edges_for( nearest_d );
            for ( auto e: edges ) {
                // Нас интересуют только взлетки.
                if ( e.way_type != AirportNetwork::WAY_RUNWAY ) continue;
                auto his_nodes = __routes.get_nodes_for( e.name );
                for ( auto n : his_nodes ) {
                    bearing = xenon::bearing( from, n.location );
                    dh = bearing - heading;
                    if ( abs(dh) <= 30.0 ) {
                        node_found = true;
                        nearest_node = n;
                        break;
                    }
                }
            }
        }

        if ( ! node_found ) {
            // Подходящего узла для указанного курса мы так и не нашли.
            Logger::log(
                "Airport::get_taxi_way_for_parking(), no nearest node was found for heading "
                + to_string( heading )
            );
            return result;
        }
        
        // Подходящий узел (начала руления) - найден. Но нам нужен - не сам узел, а его дескриптор.
        
        nearest_d = __routes.get_node_d_for( nearest_node );
        if ( nearest_d == AirportNetwork::graph_t::null_vertex() ) {
            Logger::log("Airport::get_taxi_way_for_parking, nearest node descriptor was not found");
            return result;
        };

        // Все. Запрашиваем Дейкстру.
        auto way = __routes.get_shortest_path( nearest_d, parking.location );
        int npp_name = 0;
        for ( auto descriptor : way ) {
            auto node = __routes.graph()[ descriptor ];
            waypoint_t wp;
            
            // Имена - могут быть страшненькими, передавать их по сети зазря не хочется.
            // wp.name = node.name;
            wp.name = to_string( npp_name ++ );
            
            wp.type = WAYPOINT_TAXING;
            wp.action_to_achieve = ACF_DOES_NORMAL_TAXING;
            wp.location = node.location;
            wp.location.altitude = evalution_in_meters();
            result.push_back( wp );
        }

        //  Курсы, которыми достигаются точки.
        result.at(0).incomming_heading = xenon::bearing(from, result.at(0).location);
        for ( int i=0; i < (int)result.size() - 1; i++ ) {

            auto location_from = result.at( i ).location;
            auto location_to = result.at( i + 1 ).location;

            auto direction = xenon::bearing( location_from, location_to );
            result.at( i ).outgoing_heading = direction;
            result.at( i + 1 ).incomming_heading = direction;

        }
        
        // Нулевая точка полетного плана - это еще ВПП.
        result.at( 0 ).type = WAYPOINT_RUNWAY;
        // Первая (от нуля) точка полетного плана - это уход со взлетки.
        result.at( 1 ).type = WAYPOINT_RUNWAY_LEAVED;
        
        // Последняя точка - это сама парковка.
        waypoint_t gate;
        gate.location = parking.location;
        gate.incomming_heading = parking.heading;
        gate.name = parking.name;
        gate.type = WAYPOINT_PARKING;
        gate.action_to_achieve = ACF_DOES_PARKING;
        result.push_back( gate );

    } catch ( const std::range_error & re ) {
        Logger::log("ERROR: Airport::get_taxi_way_for_parking(), range error.");
        return result;
    }

    return result;
}
