#include "../include/torrentFileInfo.hpp"
#include "../include/bencodeparse.hpp"
#include <filesystem>
#include <ctime>
#include <exception>

/// @brief NOT IMPLEMENTED
/// @param func 
referringTorrentFile::referringTorrentFile(bencodeElem(*func)()) {
    throw std::runtime_error("not implemented");
    translateFunction = func;
}
referringTorrentFile::~referringTorrentFile() = default;
info* referringTorrentFile::getInfo() const {
    throw std::runtime_error("not implemented");
    return new info();
}
std::vector<std::string> referringTorrentFile::getAnnounce() const {
    throw std::runtime_error("not implemented");
    return std::vector<std::string>();
}
std::string referringTorrentFile::getComment() const {
    throw std::runtime_error("not implemented");
    return std::string();
}
std::string referringTorrentFile::createdBy() const {
    throw std::runtime_error("not implemented");
    return std::string();
}
std::time_t referringTorrentFile::creationDate() const {
    throw std::runtime_error("not implemented");
    return std::time_t();
}
