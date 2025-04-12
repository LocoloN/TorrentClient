#include <fstream>
#include <stack>
#include <bencodeparse.hpp>
#include <algorithm>
#include <cctype> 
#include <charconv>
#include <string>
#include <bits/algorithmfwd.h>
#include <iosfwd>

using namespace std;
iparser::iparser()
{
    usedFilePath = "";
    input = std::ifstream(); 
}
iparser::iparser(const parser &param) {
    *this = param;
}
/// @brief constructs iparser and with opened file
/// @param filePath path to file
iparser::iparser(const std::filesystem::path& filePath)
{
    openFile(filePath);
}
iparser::~iparser() {
    input.close();
}
/// @brief sets openedFilePath property and initializes std::ifstream input
/// needed for opening file and checking it .torrent format
/// @param path path to .torrent file
/// @return true if everything is ok
bool iparser::openFile(const std::filesystem::path &path)
{
    if(input.is_open()) input.close();
    input.clear();
    input.open(path);
    usedFilePath = path;
    try {
        runFileChecks();
    }
    catch (const std::exception& ex) {
        input.close();
        input.clear();
        usedFilePath = "";
        return false;
    }        
    return true;
}
/// @brief checks opened file for .torrent extencion and if it is suited for read
/// @exception runtime_error if not a .torrent file, file too big, file is not open
/// @return true if everything is ok
void iparser::runFileChecks() const
{
    if(!filesystem::exists(usedFilePath)){
        throw runtime_error(usedFilePath.u8string() + string(" does not exist"));
    }
    
    if(!input.is_open()) throw std::runtime_error(usedFilePath.u8string() + " file is not open");

    string extension = usedFilePath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    if(extension != ".torrent") {
        throw std::runtime_error((usedFilePath.u8string() + " not a .torrent file"));
    }

    if(std::filesystem::file_size(usedFilePath) > (512*1024)) throw std::runtime_error(usedFilePath.string() + " file too big");

    int firstChar = input.peek();
    if (firstChar != 'd') {
        std::string errorMsg = "Invalid torrent file: expected 'd', got '";
        if (firstChar == EOF) errorMsg += "EOF";
        else errorMsg += static_cast<char>(firstChar);
        errorMsg += "'";
        throw std::runtime_error(errorMsg);
    }
}
/// @brief Checks for errors during reading
/// @return true if file is ready for reading, no errors happened and not eof
inline bool iparser::readingChecks() const
{
    return (input.good() && !input.fail() && !input.bad() && !input.eof());
}
/// @brief reads data size of chunkSize
/// @return false if end of file
std::array<char, parser::chunkSize> iparser::readChunk() {
    std::array<char, chunkSize> chunk = std::array<char, chunkSize>();
    input.read(chunk.data(), chunkSize);
    return chunk;
}
std::shared_ptr<torrentFile> iparser::getLazyTorrent()
{

    //return std::shared_ptr<torrentFile>(new lazyTorrentFile()); 
}


bencodeKeySymbols parser::getKeyFromChar(const char &param)
{
    switch (param)
    {
    case 'i':
        return bencodeKeySymbols::intstart;
    break;
    case 'l':
        return bencodeKeySymbols::liststart;
    break;
    case 'd':
        return bencodeKeySymbols::mapstart;
    break;
    case 'e':
        return bencodeKeySymbols::end;
    break;
    default:
        if(isdigit(param)) return stringstart;
    break;
    }
    throw std::runtime_error("wrong char error");
}
/// @brief used to construct bencodeElem
/// @param param string_view that represents one of bencode types 
/// @return bencodeElem
bencodeElem iparser::deserialize(const std::string_view &param) {
    
}
/// @brief used to retrieve property position from opened bencode format file
/// @param param property name
/// @exception runtime_error when reading error could not find the property
/// @return index of first character of property in file - 8:announce will return index of character '8'
/// @return 
streampos parser::getPropertyPosition(const std::string_view &param) {
    
    throw std::runtime_error("Property not found");
}

template<typename T>
bencodeKeySymbols getKeyFromType();
template<>
bencodeKeySymbols getKeyFromType<std::string>(){
    return bencodeKeySymbols::stringstart;
}
template<>
bencodeKeySymbols getKeyFromType<int>(){
    return bencodeKeySymbols::intstart;
}
template<>
bencodeKeySymbols getKeyFromType<std::vector<bencodeElem>>(){
    return bencodeKeySymbols::liststart;
}
template<>
bencodeKeySymbols getKeyFromType<std::map<std::string, bencodeElem>>(){
    return bencodeKeySymbols::mapstart;
}
/// @brief returns bencodeKeySymbols value from one of bencodeDataType variant stored types
/// @param param used to get bencodeKeySymbol
/// @return bencodeKeySymbol from param
bencodeKeySymbols parser::getStoredTypeAsKey(const bencodeElem& param){
    return std::visit([](auto &&arg) -> bencodeKeySymbols {
        return getKeyFromType<std::decay_t<decltype(arg)>>();
    }, param.data);
}
void iparser::operator= (const iparser& param)  {
    this->usedFilePath = param.usedFilePath;
    this->input = std::ifstream(usedFilePath);
}