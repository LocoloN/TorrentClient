#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <variant>
#include <array>
#include <fstream>
#include <memory>
#include <charconv>

class bencodeElem;
using bencodeList = std::vector<bencodeElem>;
using bencodeDict = std::map<std::string, bencodeElem>;
using bencodeDataType = std::variant<std::string, int, bencodeList, bencodeDict>;
enum bencodeKeySymbols
{
    stringstart = 0, // <length of string>:<string itself>
    intstart = 1, // i<integer>e
    liststart = 2, // l<other bencode types>e
    mapstart = 3, // d<bencode string><other bencode types>e
    end = 4  
};

template <typename Map>
bool compareMaps (Map const &lhs, Map const &rhs);

class bencodeElem { 
public:
    bencodeDataType data;
    bencodeElem();
    bencodeElem(const std::string&);
    bencodeElem(const int& );
    bencodeElem(const bencodeList&);
    bencodeElem(const bencodeDict&);
    void operator= (const bencodeElem&);
    bool operator== (const bencodeElem&) const;
    bool operator!= (const bencodeElem&) const;
};

class parser {
private:
    static constexpr int readingChunkSize = 4096;
    std::filesystem::path openedFilePath;
    mutable std::ifstream input;
    bool readingChecks() const;
    bool readChunk(std::array<char, readingChunkSize> &);
    friend class parserTests;
    bencodeKeySymbols getKeyFromChar(const char &param);
public:
    parser();
    parser(const std::filesystem::path filePath);
    parser(const parser &parser);
    void runFileChecks() const;
    bool openFile(const std::filesystem::path &);
    static bencodeKeySymbols getStoredTypeAsKey(const bencodeElem& param);
    /// @brief converts string to T
    /// @tparam supports int and std::string
    /// @param param to convert from.
    /// if int - "i<integer>e" 
    ///if string - "<length>:<string of giver length>"
    /// @return int or std::string
    template <typename T>
    static T bencodeToType(const std::string_view &param);
    void operator= (const parser& param);
};
template <>
inline int parser::bencodeToType<int>(const std::string_view & param)
{
    int result = 0;
    std::from_chars_result temp = std::from_chars(&param[0], &param[param.length() - 1], result);
    if(temp.ec == std::errc()) throw std::runtime_error("error on bencodeToType<int>");
    return result;
}
template <>
inline std::string parser::bencodeToType<std::string>(const std::string_view & param)
{
    return std::string(param);
}
template <>
inline bencodeDict parser::bencodeToType<bencodeDict>(const std::string_view & param)
{
    return bencodeDict();
}
template <>
inline bencodeList parser::bencodeToType<bencodeList>(const std::string_view & param)
{
    return bencodeList();
}

class info
{
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
    