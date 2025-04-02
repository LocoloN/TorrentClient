#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <memory>
#include <vector>

class info {
protected:
private:
public:
    virtual long unsigned int getPieceLength() const = 0;
    virtual void getPieces() const = 0;
    virtual std::string getName() const = 0;
    virtual std::map<long int, std::filesystem::path> getFiles() const = 0;
};
    
class torrentFile {
protected:
    std::string passkey;
    unsigned int amountOfUrls;
    std::filesystem::path filePath;
public:
    virtual ~torrentFile() = 0;
    std::filesystem::path getFilePath() const;
    virtual std::shared_ptr<info> getInfo() const = 0;
    virtual std::vector<std::string> getAnnounce() const = 0;
    virtual std::string getComment() const = 0;
    virtual std::string createdBy() const = 0;
    virtual std::time_t creationDate() const = 0;
};
