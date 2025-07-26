#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <memory>
#include <vector>

namespace TorrentClient {
    class info {
    protected:
    private:
    public:
        virtual long unsigned int piece_length() const = 0;
        virtual void pieces() const = 0;
        virtual std::string name() const = 0;
        virtual std::map<long int, std::filesystem::path> files() const = 0;
    };
        
    class torrentFile {
    protected:
        std::string passkey;
        unsigned int _amountOfUrls;
        std::filesystem::path _file_path;
    private:
    public:
        virtual ~torrentFile() = 0;
        inline std::filesystem::path file_path() const {return _file_path;};
        virtual std::shared_ptr<info> info_get() const = 0;
        virtual std::vector<std::string> announce() const = 0;
        virtual std::string comment() const = 0;
        virtual std::string created_by() const = 0;
        virtual std::time_t creation_date() const = 0;
    };
}