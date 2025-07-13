#include "bencodeparse.hpp"
#include <filesystem>

using namespace TorrentClient;
inline std::filesystem::path torrentFile::getFilePath() const{
    return filePath;
}
torrentFile::~torrentFile() {};

lazyInfo::lazyInfo()
{

}