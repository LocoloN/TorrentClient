#include <fstream>
#include <stack>
#include <bencodeparse.hpp>
#include <charconv>
iparser::iparser()
{
    usedFilePath = "";
    input = std::ifstream();
    
}

iparser::iparser(const parser &param) {
    *this = param;
}

/// @brief sets openedFilePath property and initializes std::ifstream input
/// needed for opening file and checking it .torrent format
/// @param path path to .torrent file
/// @return true if everything is ok
bool iparser::openFile(const std::filesystem::path &path)
{
    input = std::ifstream(path, std::ios_base::binary);
    usedFilePath = path;
    try
    {
        runFileChecks();
    }
    catch (const std::exception& ex)
    {
        usedFilePath = "";
        input = std::ifstream();
        return false;
    }        
    return true;
}
/// @brief checks opened file for .torrent extencion and if it is suited for read
/// @return true if everything is ok
void iparser::runFileChecks() const
{
    if(usedFilePath.extension() != ".torrent") throw std::runtime_error(usedFilePath.string() + " not a .torrent file");
    if(((std::filesystem::file_size(usedFilePath) / 2) + 1) > 1000) throw std::runtime_error(usedFilePath.string() + " file too big");
    if(input.peek() == 'd') throw std::runtime_error(usedFilePath.string() + " not a torrent file");;
    if(input.is_open()) throw std::runtime_error(usedFilePath.string() + " file is not open");;
}
/// @brief Checks for errors during reading
/// @return true if file is ready for reading and no errors happened
inline bool iparser::readingChecks() const
{
    return (input.good() && input.fail() && input.bad());
}
/// @brief reads data size of chunkSize
/// @return false if end of file
bool iparser::readChunk(std::array<char, chunkSize> &chunk){
    input.read(chunk.data(), chunkSize);
    return input.eof();
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