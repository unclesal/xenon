// *********************************************************************************************************************
// *                                                   Файл статуса IVAO                                               *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 mar 2020 at 13:00 *
// *********************************************************************************************************************

#ifdef IVAO

#include <boost/algorithm/string.hpp>

#include "ivao_status_file.h"
#include "xplane_utilities.h"

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор.                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

xenon::IVAOStatusFile::IVAOStatusFile() 
    : xenon::IVAOPeriodicallyUpdatedFile()
{
    // IVAO рекомендует перегружать файл статуса не чаще, чем один раз в сутки.
    _refreshable_interval_in_seconds = 24*3600;
    _file_name = std::string("ivao_status.txt");
    
    _host = std::string("www.ivao.aero");
    _scheme = std::string("http");
    _url = std::string("/whazzup/status.txt");
    
    __whazzup_url = __voice_url = __zipped_whazzup_url = "";
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Перекрытая функция чтения из файла.                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void xenon::IVAOStatusFile::read() {
    IVAOPeriodicallyUpdatedFile::read();
    // Начальная установка URLей.
    __whazzup_url = __voice_url = __zipped_whazzup_url = "";
    // Этим заканчивается первая строка, которую мы собираемся игнорировать.
    const std::string NOTCP_MARKER = ":NOTCP";
    // Как только прочитали - тут же идет "парзинг".
    for (auto it = _lines.begin() ; it != _lines.end(); ++it) {
        std::string one_line = * it;
        // Убираем завершающие ВК/ПС
        boost::erase_all(one_line, "\r");
        boost::erase_all(one_line, "\n");
        // пробелов и табуляций - вроде быть не должно, но на всякий случай.
        boost::erase_all(one_line, " ");
        boost::erase_all(one_line, "\t");

        // Анализ того, что мы получили в данной строке.
        if (one_line.find(";") == 0)
            // Комментарий, пропускаем строку.
            continue;
        else if (one_line.find(NOTCP_MARKER) == one_line.length() - NOTCP_MARKER.length()) {
            continue;
        } else if (one_line.find("url0") == 0) {
            // whazzup в не-сжатом варианте.
            boost::erase_all(one_line, "url0=");
            __whazzup_url = one_line;
            continue;
        } else if ( one_line.find("url1") == 0 ) {
            // voice_url
            boost::erase_all(one_line, "url1=");
            __voice_url = one_line;
            continue;
        } else if ( one_line.find("gzurl0") == 0 ) {
            boost::erase_all(one_line, "gzurl0=");
            __zipped_whazzup_url = one_line;
            continue;
        } else if (
                ( one_line.find("metar0") == 0 )
                || ( one_line.find("taf0") == 0 )
                || ( one_line.find("shorttaf0") == 0 )
                || ( one_line.find("user0") == 0 )
                || ( one_line.find("atis0") == 0 )
            ) 
            // Устаревшие значения.
            continue;
        else {
            // Не нашли, что за строка.
            std::string message = "IVAOStatusFile::read(), unhandled string " + one_line;
            XPlaneUtilities::log( message );
        }
    }    
}

#endif // IVAO
