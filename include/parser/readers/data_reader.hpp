#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <optional>

namespace TorrentClient {

    template<typename data_type = unsigned char>
    class DataReader {
    protected:
        std::filesystem::path used_file_path{""};
        inline virtual bool is_good() const = 0;
    private:
    public:
        virtual bool is_open() const noexcept = 0;
        virtual bool open_file(const std::filesystem::path &path) noexcept = 0;
        virtual std::unique_ptr<DataReader> create();
        virtual std::optional<data_type> operator[] (const size_t &index) const noexcept = 0; 
        virtual std::vector<data_type> get_block() const noexcept = 0;

    };
    
}