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
    public:
        lazyTorrentFile();
        ~lazyTorrentFile();
        std::shared_ptr<info> getInfo() const override;
        std::vector<std::string> getAnnounce() const override;
        std::string getComment() const override;
        std::string createdBy() const override;
        std::time_t creationDate() const override;

    };
}