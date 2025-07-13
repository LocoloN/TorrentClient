#pragma once

#include <ctime>
#include <filesystem>
#include <array>
#include <map>
#include "lazyTorrentFile.hpp"
#include <memory>
#include "torrentBaseTypes.hpp"
#include "bencodeparse.hpp"

namespace TorrentClient {
    class bencodeElem;

    class lazyInfo : public info {
    private:
        size_t pieceLengthPosition;
        size_t piecesPosition;
        size_t namePosition;
        size_t filesPosition;
    public:
        lazyInfo();
        virtual long unsigned int getPieceLength() const;
        virtual void getPieces() const;
        virtual std::string getName() const;
        virtual std::map<long int, std::filesystem::path> getFiles() const;
    };

    class lazyTorrentFile : public torrentFile {
    protected:  
        size_t infoPosition;
        size_t announcePosition;
        size_t commentPosition;
        size_t createdByPosition;
        size_t creationDatePosition;
        std::shared_ptr<info> refInfo;
        //translates bencode to type
        bencodeElem (*translateFunction)(void);
    public:
        lazyTorrentFile(bencodeElem (*func)(void));
        ~lazyTorrentFile();
        std::shared_ptr<info> getInfo() const;
        std::vector<std::string> getAnnounce() const;
        std::string getComment() const;
        std::string createdBy() const;
        std::time_t creationDate() const;

    };
}