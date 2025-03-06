#include <fstream>
#include <stack>
#include <bencodeparse.hpp>
#include <charconv>
parser::parser()
{
    openedFilePath = "";
    input = std::ifstream();
    sequence = std::shared_ptr<bencodeElem>(new bencodeElem(std::map<std::string, bencodeElem>()));
}
parser::parser(std::filesystem::path filePath)
{
    if(!openFile(filePath)) throw std::runtime_error("wrong file format error");
}
// parser::~parser()
// {
//     delete(sequence.get());
// }
/// @brief sets openedFilePath property and initializes std::ifstream input
/// needed for opening file and checking it .torrent format
/// @param path path to .torrent file
/// @return true if everything is ok
bool parser::openFile(const std::filesystem::path &path)
{
    input = std::ifstream(path, std::ios_base::binary);
    openedFilePath = path;
    if(!runFileChecks()) {
        openedFilePath = "";
        input = std::ifstream();
        return false;
    }
    sequence = std::shared_ptr<bencodeElem>(new bencodeElem(std::map<std::string, bencodeElem>()));
    return true;
}
/// @brief checks opened file for .torrent extencion and if it is suited for read
/// @return true if everything is ok
bool parser::runFileChecks() const
{
    bool &&result = (openedFilePath.extension() == ".torrent");
    result = (result && (input.peek() == 'd'));
    result = (result && input.is_open());
    return result;
}
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
bool parser::parseToSequence()
{
    // if(!(runFileChecks() || readingChecks())) throw new std::runtime_error("file reading error");
    // unsigned int totalChunksNeeded = (std::filesystem::file_size(openedFilePath) / 2) + 1;
    // if(totalChunksNeeded > 1000) throw std::runtime_error("");
    // std::array<char, chunkSize> &&chunk = std::array<char, chunkSize>();
    // std::filesystem::file_size(openedFilePath);
    
    // std::stack<bencodeKeySymbols> currentScope;
    // currentScope.push(bencodeKeySymbols::mapstart);
    // int temp = 0;
    // for(size_t i = 0; i < totalChunksNeeded; i++)
    // {
    //     if(!readingChecks()) throw std::runtime_error("chunk reading error");
    //     readChunk(chunk);
    //     for (size_t j = 1; j < chunkSize; j++)
    //     {
    //         currentScope.push(getKeyFromChar(chunk[i]));
    //         switch (currentScope.top())
    //         {
    //         case stringstart:
    //             size_t stringLength = 0;
    //             temp = i;
    //             while (isdigit(chunk[temp]))
    //             {
    //                 temp++;
    //             }
    //             std::from_chars_result test = std::from_chars(&chunk[i], &chunk[i + temp], stringLength);
    //             if(test.ec == std::errc()) throw std::runtime_error("string parser failed");
    //             temp++;
    //             currentScope.push(stringstart);
    //         break;

    //         case intstart:
                
    //         break;

    //         case liststart:
                
    //         break;
    //         case mapstart:
                
    //         break;
    //         case end:
    //             currentScope.pop();
    //         break;
    //         }
    //     }
        
    // }
}
bool parser::readRest(std::array<char, chunkSize> &)
{
    return false;
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
