#include <fstream>
#include <bencodeparse.hpp>

parser::parser()
{
    openedFilePath = "";
    input = std::ifstream();
    sequence = std::shared_ptr<bencodeElem>(new bencodeElem(std::map<std::string, bencodeElem>()));
}
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
    return true;
}
/// @brief checks opened file for .torrent extencion and if it is open and ready for read
/// @return true if everything is ok
bool parser::runFileChecks() 
{
    bool &&result = (openedFilePath.extension() == ".torrent");
    result = (result && (input.peek() == 'd'));
    result = (result && input.is_open());
    result = (result && input.good());
    return result;
}
/// @brief reads data size of chunkSize
/// @return false if end of file
bool parser::readChunk(std::array<char, chunkSize> &chunk){
    input.read(chunk.data(), chunkSize);
    return input.eof();
}
bool parser::parseToSequence()
{
    return true;
}
bool parser::readRest(std::array<char, chunkSize> &)
{
    return false;
}
