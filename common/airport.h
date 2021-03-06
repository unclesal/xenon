// *********************************************************************************************************************
// *                                                     Аэропорт.                                                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 12:41 *
// *********************************************************************************************************************
#pragma once

#include <map>
#include <string>
#include <vector>
#include <deque>

#include <lemon/list_graph.h>

#include "structures.h"
// Здесь он действительно нужен, не для журнализации.
#include "../simulators/xplane/xenon/xplane.hpp"
#include "utils.hpp"
#include "airport_network.h"

using namespace std;
using namespace lemon;

namespace xenon {

#pragma mark Типы аэропортов.
    const int AP_TYPE_UNKNOWN = 0;
    const int AP_TYPE_LAND = 1;
    const int AP_TYPE_SEA = 16;
    const int AP_TYPE_HELI = 17;

#pragma mark Типы частот аэропорта.
    const int FREQ_NONE = 0;
    const int FREQ_ATC_RECORDED = 1050;     const int FREQ_OLD_ATC_RECORDED = 50;
    const int FREQ_ATC_UNICOM = 1051;       const int FREQ_OLD_ATC_UNICOM = 51;
    const int FREQ_ATC_CLD = 1052;          const int FREQ_OLD_ATC_CLD = 52;
    const int FREQ_ATC_GND = 1053;          const int FREQ_OLD_ATC_GND = 53;
    const int FREQ_ATC_TWR = 1054;          const int FREQ_OLD_ATC_TWR = 54;
    const int FREQ_ATC_APP = 1055;          const int FREQ_OLD_ATC_APP = 55;
    const int FREQ_ATC_DEP = 1056;          const int FREQ_OLD_ATC_DEP = 56;

#pragma mark Типы элементов, которые могут содержать в себе узлы.
    const int CONTAINER_NONE = 0;
    const int CONTAINER_TAXIWAY = 110;
    const int CONTAINER_LINEAR_FEATURE = 120;
    const int CONTAINER_BOUNDARY = 130;

#pragma mark Типы нодов.
    const int NODE_UNKNOWN = 0;
    const int NODE_PLAIN = 111;
    const int NODE_BEZIER = 112;
    const int NODE_CLOSE_BOUNDARY = 113;
    const int NODE_CLOSE_BEZIER = 114;
    const int NODE_END_LINE = 115;
    const int NODE_END_BEZIER = 116;

    class Airport {

        public:
            /**
             * @short Наземная ВПП (RWY)
             */
            struct land_runway_t {
                // Width of runway in metres
                float width = 0.0;
                // Code defining the surface type (concrete, asphalt, etc) Integer
                // value for a Surface Type Code (see below)
                int surface_type = 0;
                // Code defining a runway shoulder surface type
                // 0=no shoulder, 1=asphalt shoulder, 2=concrete shoulder
                int shoulder_surface_type = 0;
                // Runway smoothness (not used by X-Plane yet)
                // 0.00 (smooth) to 1.00 (very rough). Default is 0.25
                float smoothness = 0.0;
                // Runway centre-line lights
                // 0=no centerline lights, 1=centre line lights
                int centre_line_light = 0;
                // Runway edge lighting (also implies threshold lights)
                // 0=no edge lights, 2=medium intensity edge lights
                int edge_light = 0;
                // Auto-generate distance-remaining signs (turn off if created manually)
                // 0=no auto signs, 1=auto-generate signs
                int distance_remaining_sign = 0;
                // Runway number (eg. “31R”, “02”). Leading zeros are required.
                // Two to three characters. Valid suffixes: “L”, “R” or “C” (or blank)
                string runway_number = "";
                
                // Здесь маленько путаница. В X-Plane это всегда "конец ВПП".
                // Хотя по смыслу имеется в виду как раз "начало", т.е.
                // ближайший торец. А нам еще нужен и противоположный конец ВПП тоже.
                
                // Latitude of runway end (on runway centerline) in decimal degrees
                // Eight decimal places supported
                double end_latitude = 0.0;
                // Longitude of runway end (on runway centerline) in decimal degrees
                // Eight decimal places supported
                double end_longitude = 0.0;
                
                location_t nearest_end_location;
                // Это как раз противоположный конец и есть, т.е. дальний,
                // если мы садимся на эту взлетку.
                location_t farest_end_location;
                
                // Length of displaced threshold in metres (this is included in implied runway length)
                // A displaced threshold will always be inside (between) the two runway ends
                //Two decimal places (metres). Default is 0.00
                float displaced_threshold_length = 0.0;
                // Length of overrun/blast-pad in metres (not included in implied runway length)
                // Two decimal places (metres). Default is 0.00
                float overrun_length = 0.0;
                // Code for runway markings (Visual, non-precision, precision)
                // Integer value for Runway Marking Code (see below)
                int markings = 0;
                // Code for approach lighting for this runway end Integer
                // value for Approach Lighting Code (see below)
                int approach_ligth = 0;
                // Flag for runway touchdown zone (TDZ) lighting
                // 0=no TDZ lighting, 1=TDZ lighting
                int tdz_ligth = 0;
                // Code for Runway End Identifier Lights (REIL)
                // 0=no REIL, 1=omni-directional REIL, 2=unidirectional REIL
                int reil_light = 0;

                location_t location() {
                    return location_t {
                        .latitude = end_latitude,
                        .longitude = end_longitude
                    };
                }
            };

            struct water_runway_t {
                // Width of runway in metres Two decimal places recommended. Must be >= 1.00
                float width = 0.0;
                // Flag for perimeter buoys 0=no buoys, 1=render buoys
                int perimeter_buoys = 0;
                // Runway number. Not rendered in X-Plane (it’s on water!)
                // Valid suffixes are “L”, “R” or “C” (or blank)
                string runway_number = "";
                // Latitude of runway end (on runway centerline) in decimal degrees
                // Eight decimal places supported
                double latitude = 0.0;
                // Longitude of runway end (on runway centerline) in decimal degrees
                //Eight decimal places supported
                double longitude = 0.0;
            };

            struct helipad_runway_t {
                // Designator for a helipad. Must be unique at an airport.
                // Usually “H” suffixed by an integer (eg. “H1”, “H3”)
                string designator = "";
                // Latitude of helipad centre in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of helipad centre in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Orientation (true heading) of helipad in degrees Two decimal places recommended
                float orientation = 0.0;
                // Helipad length in metres Two decimal places recommended (metres), must be >=1.00
                float length = 0.0;
                // Helipad width in metres Two decimal places recommended (metres), must be >= 1.00
                float width = 0.0;
                // Helipad surface code Integer value for a Surface Type Code (see below)
                int surface_code = 0;
                // Helipad markings 0 (other values not yet supported)
                int markings = 0;
                // Code defining a helipad shoulder surface type
                // 0=no shoulder, 1=asphalt shoulder, 2=concrete shoulder
                int surface_type = 0;
                // Helipad smoothness (not used by X-Plane yet) 0.00 (smooth) to 1.00 (very rough). Default is 0.25
                float smoothness = 0.0;
                // Helipad edge lighting 0=no edge lights, 1=yellow edge lights
                int edge_light = 0;
            };

            struct viewpoint_t { // code = 14
                // Latitude of viewpoint in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of viewpoint in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Height (in feet) of viewpoint above ground level Integer
                int height = 0;
                // Name of viewpoint (not used by X-Plane) Descriptive text string (optional)
                string name = "";
            };

            struct light_beacon_t { // 18
                // Latitude of beacon in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of beacon in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Code for type of light beacon. Determines colors of beacon.
                // Integer Beacon Type Code (see below)
                int type_code = 0.0;
                // Name of viewpoint (not used by X-Plane) Descriptive text string (optional)
                string name = "";

            };

            struct windsock_t { // code = 19
                // Latitude of windsock in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of windsock in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Flag for windsock lighting 0=unlit, 1=illuminated
                int flag_lighting = 0;
                // Name of viewpoint (not used by X-Plane) Descriptive text string (optional)
                string name = "";
            };

            struct sign_t { // code = 20
                // Latitude of sign in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of sign in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Orientation of sign in true degrees (heading of someone looking at sign’s front)
                // Two decimal places recommended
                float orientation = 0.0;
                // Code for sign size Integer Sign Size Code (see below)
                int size_code = 0;
                // Text to be rendered on sign front and/or back Text string
                // formatted by Sign Text Definition (see below)
                string label = "";
            };

            struct lighting_objects_t { // code = 21
                // Latitude of lighting object in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of lighting object in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Code for type of lighting object Integer Lighting Object Code (see below)
                int object_type = 0;
                // Orientation of lighting object in true degrees (looking toward object)
                // Two decimal places recommended
                float orientation = 0.0;
                // Visual glideslope angle in degrees Two decimal places.
                // 0.00 if not required. Default is 3.00
                float glidescope_angle = 0.0;
                // Associated runway number (required for VASI/PAPI, etc)
                // One to three characters
                string runway_number = "";
                // Description of lighting object (not used by X-Plane Short text string (optional)
                string description = "";
            };

            struct traffic_flow_wind_rule_t { // 1001
                // METAR reporting station (may be a remote airport, eg KSEA for KBFI)
                // ICAO code, up to 7 characters
                string metar_station = "";
                // Wind direction minimum (magnetic) 000 - 359
                int wind_direction_minimum = 0;
                // Wind direction maximum (magnetic) 000 - 359
                int wind_direction_maximum = 0;
                // Maximum wind speed. Use 999 for ‘all’ wind speeds. 0 - 999
                int wind_speed_maximum = 0;
            };

            struct traffic_flow_ceiling_rule_t { // 1002
                // METAR reporting station (may be a remote airport, eg KSEA for KBFI)
                // ICAO code, up to 7 characters
                string metar_station = "";
                // Minimum reported ceiling in feet AGL at reporting station
                // Positive integer
                int minimum_height = 0;
            };

            struct traffic_flow_visibility_rule_t { // 1003
                // METAR reporting station (may be a remote airport, eg KSEA for KBFI)
                // ICAO code, up to 7 characters
                string metar_station = "";
                // Minimum reported visibility in statute miles Float (eg. “1.5”)
                float minimum_visibility = 0.0;
            };

            struct traffic_flow_runway_in_use_t { // 1100, 1110
                // Runway end identifier Two to three characters. Valid suffixes: “L”, “R” or “C” (or blank)
                string runway = "";
                // Arrival or departure frequency. rounded DOWN where necessary, 1kHz resolution
                int frequency;
                // Rule type (arrivals, departures) Pipe separated list (“|”). ‘arrivals’ and/or ‘departures’
                string rule_type = "";
                // Airplane types to which rule applies Pipe-separated list (“|”).
                // Can include “heavy”, ”jets”, “turboprops”, “props” and “helos”
                string airplane_types = "";
                // On course heading range ((ie. first leg of flight plan
                // for departures, last leg for arrivals)
                // 000000 - 359359
                string on_course_heading = "";
                // Initial ATC assigned departure heading range. Not used for arrivals.
                // 000000 - 359359
                string atc_heading = "";
                // Rule name Descriptive name (max 50 characters)
                string name = "";
            };

            struct traffic_flow_t { // 1000
                string name = "";
                vector< traffic_flow_wind_rule_t > wind_rules; // 1001
                vector< traffic_flow_ceiling_rule_t > ceiling_rules; // 1002
                vector< traffic_flow_visibility_rule_t> visibility_rules; // 1003
                string time_from = "";
                string time_to = "";
                // RWY - direction
                map<string, string> vfr_pattern;
                vector< traffic_flow_runway_in_use_t > runway_in_use;
            };

            struct frequency_t { // 1050 - 1056, 50-56
                // Сам исходный код того, что привело к образованию "частоты".
                int freq_type = FREQ_NONE;
                int frequency = 0.0;
                string description = "";
            };

            struct taxi_network_routing_edge_active_zone_t {
                string classification;
                string runways;
            };

            struct truck_parking_t { // 1400
                // Latitude of location in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of location in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Heading (true) of the OBJ positioned at this location Decimal degrees
                float heading = 0.0;
                // type string baggage_loader, baggage_train, crew_car, crew_ferrari, crew_limo, pushback,
                // fuel_liners, fuel_jets, fuel_props, food, gpu
                string parking_type = "";
                // 0 to 10 if type is baggage_train, 0 if not
                int baggage_train_type = 0.0;
                // Name of parking Text string
                string name = "";
            };

            struct truck_destination_t { // 1401
                // Latitude of location in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of location in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Heading (true) of the positioned at this location Decimal Degrees, true heading
                float heading = 0.0;
                // Truck types allowed to end up at this destination Pipe separated list (“|”).
                // Include 1 or more of the following baggage_loader,
                // baggage_train, crew_car, crew_ferrari, crew_limo, pushback, fuel_liners, fuel_jets,
                // fuel_props, food, gpu
                string truck_types = "";
                // Name of Truck Destination Text string
                string name = "";
            };

            struct node_t {
                // Row code for a node. First node must follow an appropriate header row
                // 111 thru 116
                int node_type = NODE_UNKNOWN;
                // [All nodes] Latitude of node in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // [All nodes] Longitude of node in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // [112, 114, 116 only] Latitude of Bezier control point in decimal degrees
                // Eight decimal places supported. Ignore for 111, 113, 115
                double bezier_point_latitude = 0.0;
                // [112, 114, 116 only] Latitude of Bezier control point in decimal degrees
                // Eight decimal places supported. Ignore for 111, 113, 115
                double bezier_point_longitude = 0.0;
                // [Not for 115 or 116] Code for painted line type on line segment starting at this node
                // Integer Line Type Code (see below). Not for 115 or 116
                int line_type_code = 0;
                // [Not for 115 or 116] Code for lighting on line segment starting at this node
                // Integer Line Type Code (see below). Not for 115 or 116
                int lighting_type_code = 0;

            };

            struct node_container_t {
                int container_type = CONTAINER_NONE;
                vector< node_t > nodes;
            };

            /**
             * @short Taxiways, node container.
             */
            struct taxiway_t : public node_container_t {
                // Code defining the surface type (concrete, asphalt, etc)
                // Integer value for a Surface Type Code (see below)
                int surface_type = 0;
                // Runway smoothness (not used by X-Plane yet)
                // 0.00 (smooth) to 1.00 (very rough). Default is 0.25
                float smoothness = 0.0;
                // Orientation (true degrees) of pavement texture ‘grain’ Two decimal places recommended
                float orientation = 0.0;
                // Description of pavement chunk (not used by X-Plane) Text string
                string description = "";
            };

            /**
             * @short Linear feature, node container.
             */
            struct linear_feature_t : public node_container_t {
                string name;
            };

            /**
             * @short Airport boundary, node container.
             */
            struct boundary_t : public node_container_t {
                // Description of feature or boundary (not used by X-Plane). Text string.
                string description;
            };

            Airport();
            Airport( const Airport & apt ) {
                * this = apt;
            };
            Airport & operator = ( const Airport & apt ) = default;
            virtual ~Airport() = default;
            // Получить ближайший по координатам.
            static void get_nearest( location_t & location );
            // Получить по ICAO
            static Airport & get_by_icao( const string & icao_code );
            // Чтение всех имеющихся сценариев.
            static void read_all();

            /**
             * @short Прочитать один файл формата apt.dat и распарзить его.
             * https://developer.x-plane.com/article/airport-data-apt-dat-file-format-specification/
             * Возвращаемого значения нет, вместо этого функция замещает элемент в Airport::_airports_. Аэропорт
             * в файле может быть не один, парзится - либо до окончания файла, либо до получения типа 99
             * (признак конца файла).
             *
             * @throw xenon::bad_format_exception В этом случае дальнейшая обработка файла прекращается,
             * но при этом в словарь Airport::_airports_ будут записаны те аэропорты, которые успели распарзится.
             * @throw runtime_error если были ошибки в логике парзинга.
             */
            static void read( const string & full_path_to_apt_dat );

            /** @short Вернуть общее количество распарзенных аэропортов.
             *
             * @return
             */
            static int count() {
                return Airport::__airports.size();
            };

            /**
             * @short Вернуть флаг прочитанности всех аэропортов (т.е. инициализированности класса).
             * @return
             */
            static bool airports_was_readed() {
                return __airports_was_readed;
            };

            string icao_code() {
                return __icao_code;
            };

            /**
             * Установка указателя на cb-функцию после распознавания аэропорта.
             * @param reaction
             */
            static void set_has_been_parsed( void ( * reaction )( string & icao_code ) ){
                Airport::__has_been_parsed = reaction;
            }

            /**
             * @short "Осиротевшесть" аэропорта.
             * @return
             * @see _orphanded
             */

            bool is_orphanded() {
                return __orphanded;
            }

            const xenon::AirportNetwork & routes() {
                return __routes;
            }

            /**
             * @short Высота аэропорта в метрах.
             */
            double evalution_in_meters() {
                return xenon::feet_to_meters( __evalution_in_feet );
            };

            /**
             * @short Вернуть ссылку на все имеющиеся стоянки.
             * @return
             */

            map< string, startup_location_t > & get_startup_locations() {
                return __startup_locations;
            }

            /**
             * @short Вернуть используемые ВПП c указанием их выполняемой функции (departure, arrival)
             * @return
             */
            vector<runway_in_use_t> get_runways_in_use();

            /**
             * Получить объект ВПП для конкретного действия (взлета или посадки).
             * @param use
             * @return
             */
            land_runway_t get_runway_for( const runway_used_t & use );

            /**
             * @short Выдать путь для рулежки к полосе, с которой производится взлет.
             * @param from точка, где сейчас самолет находится. Курс для расчета конечной (целевой) точки -
             * не имеет значения, т.к. первоначальный сегмент все равно будет прямолинейным. Курс влияет
             * на то, будет ли самолет выруливать сам или его надо выталкивать, но это решает уже сам
             * самолет.
             */

            deque< waypoint_t > get_taxi_way_for_departure( const location_t & from );

            /**
             * @brief Вернуть путь руления от указанного места до стоянки.
             * @param from Точка, от которой рулим.
             * @param heading Курс, которым мы сейчас стоИм, чтобы исключить точки сзади ВС.
             * @param parking Стоянка, на которую хотим попасть.
             * @return Путь, если он был найден. Первая точка этого пути ищется в растре 30
             * градусов от указанного курса. И если она не найдена, если куср был каким-нибудь
             * "левым" - то возвращаемый путь окажется пустым.
             */
            deque< waypoint_t > get_taxi_way_for_parking(
                const location_t & from,
                const float & heading,
                const startup_location_t & parking
            );

            startup_location_t get_free_parking( const std::string & icao_type );

        private:

            // Общая кучка прочитанных аэропортов. Ключом является код аэропорта,
            // таким образом, более поздние замещают более ранних.
            static map<string, Airport> __airports;
            static bool __airports_was_readed;
            // Пустышка. Ссылка на нее выдается в том случае,
            // если не был найден запрошенный аэропорт.
            static xenon::Airport __fake_airport;

            // На всякий случай запоминается полный путь к исходному распарзенному файлу.
            string __full_path_to_apt_dat;
            string __origin;
            int __version;
            int __ap_type;
            int __evalution_in_feet;
            string __icao_code;
            string __name;
            viewpoint_t __viewpoint;

            // Атрибуты аэропорта. Могут быть, а могут не быть.
            string __city;
            string __country;
            double __datum_lon;
            double __datum_lat;
            string __gui_label;
            string __iata_code;
            string __region_code;
            string __state;
            string __transition_alt;
            string __transition_level;
            string __faa_code;
            int __flatten;
            bool __drive_on_left;
            string __local_code;

            /**
             * @short "Осиротевшесть" аэропорта внутри X-Plane.
             * В нормальном состоянии аэропорт представляет собой отдельно действующего агента,
             * который общается с X-Plane через сервер или P2P. Если внешний агент недоступен,
             * то аэропорт внутри X-Plane - "осиротел" и действует сам по себе.
             */
            bool __orphanded;

            // Реакция на парзинг одного аэропорта.
            static void ( * __has_been_parsed )( string & icao_code );

            // Граф taxi network внутри объекта аэропорта.
            xenon::AirportNetwork __routes;

            // Объекты внутри аэропорта.
            vector< land_runway_t > __land_runways;
            vector< water_runway_t > __water_runways;
            vector< helipad_runway_t > __helipad_runways;
            // Ключ - имя стоянки. Элемент - сама стоянка.
            map< string, startup_location_t > __startup_locations;
            vector< truck_parking_t > __truck_parkings;
            vector< truck_destination_t > __truck_destinations;

            // node container's
            vector< light_beacon_t > __light_beacons;
            vector< frequency_t > __frequencies;
            vector< windsock_t > __windsocks;
            vector< sign_t > __signs;
            vector< lighting_objects_t > __lighting_objects;
            vector< taxiway_t > __taxiways;
            vector< linear_feature_t > __linear_features;
            vector< boundary_t > __boundaries;

            // Traffic flow rules
            vector <traffic_flow_t> __traffic_flow;

            // Положить насчитанный контейнер в аэропорт и освободить динамически созданный блок в памяти.
            void __put_node_container( node_container_t ** ptr_container );
            void __put_traffic_flow( traffic_flow_t ** ptr_traffic );
            void __put_startup_location( startup_location_t ** ptr_startup_location );
            static void __put_airport( Airport ** ptr_airport );

            /**
             * @short Проверить "полноту" заполнения ВПП.
             * Бывает, что взлетки в файле - все полностью. А бывает, что дают только один конец этих взлеток,
             * второй нужно вычислять по taxi network. Функция дополняет ВПП аэропорта, если их не хватает.
             */

            void __check_runway_fullness();

    };

};
