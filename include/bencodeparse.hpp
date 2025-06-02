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


class bencodeElem;
class bencodeElem { 
public:
    std::unique_ptr<bencodeDataType> data;
    bencodeElem();
    bencodeElem(const bencodeElem &arg);
    bencodeElem(bencodeElem &&arg);
    bencodeElem(const std::string&);
    bencodeElem(const int& );
    bencodeElem(const bencodeList&);
    bencodeElem(const bencodeDict&);
    bencodeKeySymbols getStoredTypeAsKey() const;
    void operator= (const bencodeElem&);
    void operator= (bencodeElem &&);
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
    virtual inline bool is_open() const = 0;
    /// @brief converts string to T
    /// @tparam supports int and std::string
    /// @param param to convert from.
    /// if int - "i<integer>e" 
    ///if string - "<length>:<string of giver length>"
    /// @return int or std::string
    // template <typename T>
    // static T bencodeToType(const std::string_view &param);
};
class iparser : virtual public parser {
private:
    mutable std::ifstream input;
    friend class iparserTests;
    /// @brief Checks for errors during reading
    /// @return true if file is ready for reading, no errors happened and not eof
    inline bool readingChecks() const
    {
        return input.good() && !input.eof();
    }
    /// @brief reads data size of chunkSize
    std::unique_ptr<char[]> readChunk() const;
public:
    iparser();
    iparser(const std::filesystem::path &filePath);
    iparser(const parser &parser);
    ~iparser() override;
    /// @brief used to construct bencodeElem from string
    /// @param param string_view that represents one of bencode types 
    /// @return bencodeElem
    std::shared_ptr<torrentFile> getLazyTorrent();
    /// @brief used to retrieve property position from opened bencode format file
    /// @param param property name
    /// @exception runtime_error when reading error could not find the property
    /// @return index of first character of property in file - 8:announce will return index of character '8'
    /// @return 
    std::streampos getPropertyPosition(const std::string_view &param) const;
    /// @brief checks opened file for .torrent extension and if it is suited for reading
    /// @exception runtime_error if not a .torrent file, file too big, file is not open
    /// @return true if everything is ok
    void runFileChecks() const;
    inline bool is_open() const {
        return input.is_open();
    }  
    bool openFile(const std::filesystem::path &);
    void operator= (const iparser& param);
};
class oparser : virtual public parser {
    
};
bencodeKeySymbols getKeyFromChar(const char &param);
bencodeElem deserialize(const std::string_view &param);
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

