#pragma once

#include <parser/readers/DataReader.hpp>
#include <fstream>

using namespace std;
using namespace TorrentClient;

class SimpleDataReader : public DataReader<> {
protected:
    mutable ifstream input; 
    inline bool reading_checks() const noexcept;
    SimpleDataReader() = default;
private:
public:
    bool is_open() const noexcept override{ 
        return input.is_open(); 
    } 
    bool open_file(const std::filesystem::path &path) noexcept override;
    bool close() noexcept override;
    optional<unsigned char> operator[] (const std::size_t &index) const noexcept; 
    optional<vector<unsigned char>> get_block(const size_t offset, size_t size) const noexcept; 

    

}; 
