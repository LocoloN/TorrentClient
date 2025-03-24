#pragma once

#include <ctime>
#include <filesystem>
#include "bencodeparse.hpp"
#include <array>
#include <map>

struct lazyInfo : info
{

    size_t pieceLengthPosition;
    size_t piecesPosition;
    size_t namePosition;
    size_t filesPosition;
};

class lazyTorrentFile : torrentFile {
protected:  
    size_t infoPosition;
    size_t announcePosition;
    size_t commentPosition;
    size_t createdByPosition;
    size_t creationDatePosition;
    std::shared_ptr<info> refInfo;
    //translates bencode to type
    bencodeElem (*translateFunction)();
public:
    lazyTorrentFile(bencodeElem(*func)());
    ~lazyTorrentFile();
    std::shared_ptr<info> getInfo() const;
    std::vector<std::string> getAnnounce() const;
    std::string getComment() const;
    std::string createdBy() const;
    std::time_t creationDate() const;
};
