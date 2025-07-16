#include "lazyTorrentFile.hpp"
#include "../include/bencodeparse.hpp"
#include <filesystem>
#include <ctime>
#include <exception>

using namespace TorrentClient;

/// @brief NOT IMPLEMENTED
/// @param func 
lazyTorrentFile::lazyTorrentFile() {}
lazyTorrentFile::~lazyTorrentFile() = default;
std::vector<std::string> lazyTorrentFile::announce() const {
    throw std::runtime_error("not implemented");
    return std::vector<std::string>();
}
std::shared_ptr<info> lazyTorrentFile::info_get() const {
    throw std::runtime_error("not implemented");
    
}
std::string lazyTorrentFile::comment() const {
    throw std::runtime_error("not implemented");
    return std::string();
}
std::string lazyTorrentFile::created_by() const {
    throw std::runtime_error("not implemented");
    return std::string();
}
std::time_t lazyTorrentFile::creation_date() const {
    throw std::runtime_error("not implemented");
    return std::time_t();
}
