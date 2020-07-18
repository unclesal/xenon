// *********************************************************************************************************************
// *                                              Common program's settings.                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 13 mar 2019 at 22:07 *
// *********************************************************************************************************************
#pragma once

// Name of my "organization" for program settings store and restore.
#define XENON_ORGANIZATION_NAME             "UncleSal"

// One-hop multicast address for broadcast UDP messages.
#define NETWORK_MULTICAST_GROUP  "224.0.0.111"

// Plugin port
#define NETWORK_PORT 14242

// Хост, на котором расположен сервер коммуникатора.
#define COMMUNICATOR_HOST "localhost"

// Порт TCP-коммуникатора (выделенного сервера общения агентов между собой)
#define COMMUNICATOR_PORT 14243

// Максимальное число клиентов, которые может обслужить один коммуникатор.
#define COMMUNICATOR_MAX_CLIENTS 100

// Максимальный размер пакета, который мы собрались обрабатывать.
#define COMMUNICATOR_MAX_PACKET_SIZE  16384 // 32768

// The frequency of sending to the network the status of the
// aircrafts by default, Hertz (times per second).

#define USER_AIRCRAFT_STATE_FREQUENCY 1.5

// Частота опроса состояния окружающей среды, раз в секунду (Гц).
#define AMBIENT_STATE_FREQUENCY 1.0

// Частота опроса состояния автопилота, раз в секунду (Гц).
#define AUTOPILOT_STATE_FREQUENCY 2.0

// Частота опроса неблокирующей сетевой сокеты, раз в секунду (Гц)
#define NETWORK_SOCKET_FREQUENCY 2.0

// Один "тик" цикла агента в микросекундах.
constexpr int AGENT_TICK = 10000;
    
// Количество циклов (тиков), после которых агент сообщает о 
// своем новом местоположении и параметрах.
constexpr unsigned int CYCLES_PER_SCREAM = 150;


// Частота опроса и устаровки состояния Bimbo-самолетов, раз в секунду (Гц)
// #define BIMBO_AIRCRAFT_FREQUENCY 10.0

// Время "сна" в цикле получения whazzup, в секундах.
// Это - не то же самое, что время рефреша файла. Просто поспали - пошли дальше.
// Далеко не факт, что из "пошли дальше" сразу же последует "запросили файл".
// #define WHAZZUP_SLEEP_SECONDS   30
