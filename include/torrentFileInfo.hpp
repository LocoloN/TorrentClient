#pragma once

#include <ctime>
#include <filesystem>
#include "bencodeparse.hpp"
#include <array>
#include <map>

struct referringInfo : info
{
    
    size_t pieceLengthPosition;
    size_t piecesPosition;
    size_t namePosition;
    size_t filesPosition;
};

class referringTorrentFile : torrentFile {
protected:  
    size_t infoPosition;
    size_t announcePosition;
    size_t commentPosition;
    size_t createdByPosition;
    size_t creationDatePosition;
    referringInfo refInfo;
    //translates bencode to type
    bencodeElem (*translateFunction)();
public://
    referringTorrentFile(bencodeElem(*func)());
    ~referringTorrentFile();
    info* getInfo() const;
    std::vector<std::string> getAnnounce() const;
    std::string getComment() const;
    std::string createdBy() const;
    std::time_t creationDate() const;
};
