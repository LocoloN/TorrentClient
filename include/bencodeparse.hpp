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
using bencodeDataType = std::variant<std::string, int, std::vector<bencodeElem>, std::map<std::string, bencodeElem>>;
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
    bencodeElem(const std::vector<bencodeElem>&);
    bencodeElem(const std::map<std::string, bencodeElem>&);
    
    /// @brief initialises with std::string
    /// @return pointer to data
    bencodeDataType& stringInit();
    /// @brief initialises with int
    /// @return this
    bencodeDataType& intInit();
    /// @brief initialises with std::vector<bencodeElem>
    /// @return this
    bencodeDataType& vectorInit();
    /// @brief initialises with std::map<std::string, bencodeElem>
    /// @return this
    bencodeDataType& mapInit();

    

    void operator= (const bencodeElem&);
    bool operator== (const bencodeElem&) const;
    bool operator!= (const bencodeElem&) const;
};
class parser {
private:
    //~parser();
    static constexpr size_t chunkSize = 4096;
    std::vector<char> buffer;
    std::filesystem::path openedFilePath;
    mutable std::ifstream input;
    bool readingChecks() const;
    bool readChunk(std::array<char, chunkSize> &);
    bool readRest(std::array<char, chunkSize> &); // useless
    friend class parserTests;
public:
    std::shared_ptr<bencodeElem> sequence;
    parser();
    parser(const std::filesystem::path filePath);
    bool runFileChecks() const;
    bool openFile(const std::filesystem::path &);
    bool parseToSequence();
    static bencodeKeySymbols getKeyFromChar(const char &param);
    static bencodeKeySymbols getStoredTypeAsKey(const bencodeElem& param);
    /// @brief converts string to T
    /// @tparam supports int and std::string
    /// @param param to convert from.
    /// if int - "i<integer>e" 
    ///if string - "<length>:<string of giver length>"
    /// @return int or std::string
    template <typename T>
    static T bencodeToType(const std::string_view &param);
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

