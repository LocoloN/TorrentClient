#pragma once

#include <parser/readers/DataReader.hpp>
#include <fstream>

using namespace std;
using namespace TorrentClient;

class SimpleDataReader_tests;
//class SimpleDataReader;

namespace TorrentClient {

    class SimpleDataReader : virtual public DataReader {
    protected:
        mutable ifstream input;
        inline bool reading_checks() const noexcept;
        bool is_good() const override;
        friend class SimpleDataReader_tests;
    private:
    public:
        SimpleDataReader() = default;
        virtual ~SimpleDataReader() = default;
        bool is_open() const noexcept override{ 
            return input.is_open(); 
        } 
        bool open_file(const std::filesystem::path &path) noexcept override;
        void close_file() noexcept override;
        optional<unsigned char> operator[] (const std::size_t &index) const noexcept; 
        optional<vector<unsigned char>> get_block(size_t offset, size_t size) const noexcept; 
    }; 
}