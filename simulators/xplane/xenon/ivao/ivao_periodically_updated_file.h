// *********************************************************************************************************************
// *                                          Периодически обновляемый файл                                            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 mar 2020 at 12:02 *
// *********************************************************************************************************************
#pragma once

#ifdef IVAO

#include <string>
#include <vector>

namespace xenon {
    class IVAOPeriodicallyUpdatedFile {
        public:            
            IVAOPeriodicallyUpdatedFile();
            ~IVAOPeriodicallyUpdatedFile() = default;
            
            /** Можно ли уже перегрузить данный файл?
             */
            bool can_be_updated();
            
            /** Интервал в секундах, через который данный файл можно было бы и перегрузить.
             */
            int get_refresh_interval() {
                return _refreshable_interval_in_seconds;
            };
            
            /** Выдать хост, откуда загружается этот файл
             */
            std::string host() {
                return _host;
            };
            
            /** Выдать схему, по которой можно получить этот файл.
             */
            std::string scheme() {
                return _scheme;
            };
            
            /** Выдать URL, с которого можно забрать этот файл.
             */
            std::string url() {
                return _url;
            };
            
            /** Выдать собственный полный путь.
             */
            std::string path();
                        
            // Получить содержимое файла с WEBа.
            virtual std::string request_from_web();
            
            /** Перезагрузить файл с вэба и записать его на диск.
             * @return был ли реально переписан файл.
             */
            bool reload();
            
            virtual void read();
            
            /** GZIP compression
             */
            std::string gzip_encode(const std::string & data);
            
            /** GZIP decompression.
             */
            std::string gunzip_decode(const std::string & data);
            
        protected:
            
            // Интервал в секундах, через который происходит перезагрузка данного файла.
            int _refreshable_interval_in_seconds;
            // Имя файла (переставляется в классах-потомках).
            std::string _file_name;
            // Хост, с которого можно забрать этот файл.
            std::string _host;
            // Схема получения файла.
            std::string _scheme;
            // URL для данного файла.
            std::string _url;
            
            /** Массив (вектор) прочитанных строк файла. 
             * Строки перезаполняются при вызове метода read()
             * @see read()
             */
            std::vector<std::string> _lines;
    };
    
}; // namespace xeon

#endif // IVAO
