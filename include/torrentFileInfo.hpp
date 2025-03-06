#pragma once

#include <ctime>
#include <filesystem>
#include "bencodeparse.hpp"
#include <array>
#include <map>

class torrentFile
{
private:
    std::string passkey;
    unsigned int amountOfUrls;
    std::filesystem::path FilePath;
    std::shared_ptr<bencodeElem> tree;
public:
    TorrentInfo getInfo();
    std::vector<std::string> getAnnounce();
    std::string getComment();
    std::string createdBy();
    std::time_t creationDate();
    
};

struct TorrentInfo
{
    long int pieceLength;
    std::string pieces;
    std::string name;
    std::map<long int, std::string> files;
};