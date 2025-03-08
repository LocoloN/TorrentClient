#include "../include/bencodeparse.hpp"
#include <filesystem>

inline std::filesystem::path torrentFile::getFilePath() const{
    return filePath;
}
torrentFile::~torrentFile() {};