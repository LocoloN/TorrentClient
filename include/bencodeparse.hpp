#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <variant>
#include <array>
#include <fstream>
#include <memory>

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

bencodeKeySymbols getStoredTypeAsKey(const bencodeElem& param);
bencodeKeySymbols getKeyFromChar(const char param);

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
    static constexpr size_t chunkSize = 4096;
    std::vector<char> buffer;
    std::filesystem::path openedFilePath;
    mutable std::ifstream input;
    bool readingChecks() const;
    bool readChunk(std::array<char, chunkSize> &);
    bool readRest(std::array<char, chunkSize> &);
    friend class parserTests;
    public:
    std::shared_ptr<bencodeElem> sequence;
    parser();
    parser(const std::filesystem::path filePath);
    bool runFileChecks() const;
    bool openFile(const std::filesystem::path &);
    bool parseToSequence();
};
