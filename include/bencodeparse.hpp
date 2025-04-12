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
#include "lazyTorrentFile.hpp"
#include "torrentBaseTypes.hpp"

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
protected:
    static constexpr int chunkSize = 4096;
    std::filesystem::path usedFilePath;
public:
    virtual ~parser() = default;
    virtual bool openFile(const std::filesystem::path &) = 0;
    std::streampos getPropertyPosition(const std::string_view &param);
    static bencodeKeySymbols getStoredTypeAsKey(const bencodeElem& param);
    /// @brief converts string to T
    /// @tparam supports int and std::string
    /// @param param to convert from.
    /// if int - "i<integer>e" 
    ///if string - "<length>:<string of giver length>"
    /// @return int or std::string
    // template <typename T>
    // static T bencodeToType(const std::string_view &param);
    static bencodeKeySymbols getKeyFromChar(const char &param);
};
class iparser : virtual public parser {
private:
    mutable std::ifstream input;
    friend class iparserTests;
    bool readingChecks() const;
    std::array<char, chunkSize> readChunk();
public:
    iparser();
    iparser(const std::filesystem::path &filePath);
    iparser(const parser &parser);
    ~iparser() override;
    static bencodeElem deserialize(const std::string_view &param);
    std::shared_ptr<torrentFile> getLazyTorrent();
    void runFileChecks() const;
    bool openFile(const std::filesystem::path &);
    void operator= (const iparser& param);

};
class oparser : virtual public parser {

};
// template <>
// inline int parser::bencodeToType<int>(const std::string_view & param)
// {
//     int result = 0;
//     std::from_chars_result temp = std::from_chars(&param[0], &param[param.length() - 1], result);
//     if(temp.ec == std::errc()) throw std::runtime_error("error on bencodeToType<int>");
//     return result;
// }
// template <>
// inline std::string parser::bencodeToType<std::string>(const std::string_view & param)
// {
//     return std::string(param);
// }
// template <>
// inline bencodeDict parser::bencodeToType<bencodeDict>(const std::string_view & param)
// {
//     return bencodeDict();
// }
// template <>
// inline bencodeList parser::bencodeToType<bencodeList>(const std::string_view & param)
// {
//     return bencodeList();
// }

