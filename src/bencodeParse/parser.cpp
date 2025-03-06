#include <fstream>
#include <stack>
#include <bencodeparse.hpp>
#include <charconv>
parser::parser()
{
    openedFilePath = "";
    input = std::ifstream();
    
}
parser::parser(std::filesystem::path filePath)
{
    if(!openFile(filePath)) throw std::runtime_error("wrong file format error");
}
parser::parser(const parser &param) {
    *this = param;
}

/// @brief sets openedFilePath property and initializes std::ifstream input
/// needed for opening file and checking it .torrent format
/// @param path path to .torrent file
/// @return true if everything is ok
bool parser::openFile(const std::filesystem::path &path)
{
    input = std::ifstream(path, std::ios_base::binary);
    openedFilePath = path;
    try
    {
        runFileChecks();
    }
    catch (const std::exception& ex)
    {
        openedFilePath = "";
        input = std::ifstream();
        return false;
    }        
    return true;
}
/// @brief checks opened file for .torrent extencion and if it is suited for read
/// @return true if everything is ok
void parser::runFileChecks() const
{
    if(openedFilePath.extension() != ".torrent") throw std::runtime_error(openedFilePath.string() + " not a .torrent file");
    if(((std::filesystem::file_size(openedFilePath) / 2) + 1) > 1000) throw std::runtime_error(openedFilePath.string() + " file too big");
    if(input.peek() == 'd') throw std::runtime_error(openedFilePath.string() + " not a torrent file");;
    if(input.is_open()) throw std::runtime_error(openedFilePath.string() + " file is not open");;
}
/// @brief Checks for errors during reading
/// @return true if file is ready for reading and no errors happened
inline bool parser::readingChecks() const
{
    return (input.good() && input.fail() && input.bad());
}
/// @brief reads data size of chunkSize
/// @return false if end of file
bool parser::readChunk(std::array<char, chunkSize> &chunk){
    input.read(chunk.data(), chunkSize);
    return input.eof();
}
/// @brief parses opened file to a bencodeElem tree
/// @return std::shared_ptr<bencodeElem> to first element of tree, NULL if fail
std::shared_ptr<bencodeElem> parser::parseToTree()
{
    runFileChecks();
    try
    {
        runFileChecks();
    }
    catch (const std::exception& ex)
    {
        return NULL;
    }
    std::shared_ptr<bencodeElem> result(new bencodeElem());
    unsigned int totalChunksNeeded = (std::filesystem::file_size(openedFilePath) / 2) + 1;
    std::array<char, chunkSize> &&chunk = std::array<char, chunkSize>();
    std::stack<bencodeKeySymbols> currentScope;
    result->mapInit();
    currentScope.push(mapstart);

    int temp = 0;
    for(size_t i = 0; i < totalChunksNeeded; i++)
    {
        if(!readingChecks()) throw std::runtime_error("chunk reading error");
        readChunk(chunk);
        for (size_t j = 1; j < chunkSize; j++)
        {
            currentScope.push(getKeyFromChar(chunk[i]));
            switch (currentScope.top())
            {
            case stringstart:
                
            break;

            case intstart:
                
            break;

            case liststart:
                
            break;
            case mapstart:
                
            break;
            case end:
                currentScope.pop();
            break;
            }
        }
    }
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
void parser::operator= (const parser& param) {
    this->openedFilePath = param.openedFilePath;
    this->input = std::ifstream(openedFilePath);
}