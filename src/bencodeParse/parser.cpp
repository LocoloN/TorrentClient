#include <fstream>
#include <stack>
#include <bencodeparse.hpp>
#include <algorithm>
#include <cctype> 
#include <charconv>
#include <string>
#include <iterator>
#include <bits/algorithmfwd.h>
#include <iosfwd>

using namespace std;
using namespace TorrentClient;

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
    input.open(path, std::ios_base::binary);
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
unique_ptr<char[]> iparser::readChunk() const {
    unique_ptr<char[]> chunk = make_unique<char[]>(chunkSize);
    input.read(chunk.get(), chunkSize);
    return chunk;
}
std::shared_ptr<torrentFile> iparser::getLazyTorrent()
{

    //return std::shared_ptr<torrentFile>(new lazyTorrentFile()); 
}
streampos iparser::getPropertyPosition(const std::string_view &param) const {
    if(param.empty()) return streampos(-1);
    if(param.size() > chunkSize) throw runtime_error(string("search parameter cant be longer than ") + to_string(chunkSize));
    if(!input.is_open()) throw runtime_error("getPropertyPosition error, " + usedFilePath.string() += " is not open");
    int totalSize = param.size() + chunkSize;
    struct streamState {
        ifstream &stream;
        streampos initialPos;
        ios_base::iostate initialFlag;

        streamState(ifstream &_input)
          : stream(_input),
            initialPos(_input.tellg()),
            initialFlag(_input.rdstate()) { }
        ~streamState() {
            stream.clear();
            stream.seekg(initialPos);
            stream.setstate(initialFlag);
        }
    } streamSaver(input);
    input.seekg(0, ios::beg);
    input.clear();

    unique_ptr<char[]> chunk = make_unique<char[]>(totalSize);
    std::fill_n(chunk.get(), param.size(),'0');
    char* begin;
    char* end;
    char* posIter;
    unique_ptr<char[]> overlapBuffer = make_unique<char[]>(param.size());
    //data that you get when reading a chunkSize of bytes from file
    std::unique_ptr<char[]> data;

    data = readChunk();
    while(readingChecks())
    {
        copy(data.get(), (data.get() + input.gcount()), (chunk.get() + param.size()));
        
        begin = chunk.get();
        end = begin + param.size() + input.gcount();
        
        posIter = std::search(begin, end,param.begin(), param.end());
        if(posIter != end) {
            return (input.tellg() - input.gcount() - static_cast<streamoff>(param.size())) + (posIter - begin);
        }
        
        copy(end - param.size(), end, overlapBuffer.get());
        copy(overlapBuffer.get(), overlapBuffer.get() + param.size(), chunk.get());

        data = readChunk();
    }
    if(input.gcount() > 0)
    {
        input.clear();

        begin = chunk.get();
        end = begin + param.size() + input.gcount();

        copy(data.get(), (data.get() + input.gcount()), (chunk.get() + param.size()));
        copy(overlapBuffer.get(), overlapBuffer.get() + param.size(), chunk.get());

        posIter = std::search(begin, end,param.begin(), param.end());
        if(posIter != end) {
            return (input.tellg() - input.gcount() - static_cast<streamoff>(param.size())) + (posIter - begin);
        }
    }
    return streampos(-1);
}
void iparser::operator= (const iparser& param)  {
    this->usedFilePath = param.usedFilePath;
    this->input = std::ifstream(usedFilePath);
}
bencodeElem TorrentClient::deserialize_simple(const std::string_view &param) {
    size_t current_indent = 1;
    switch (getKeyFromChar(param[0]))
    {
    case bencodeKeySymbols::stringstart : {
        
        size_t delimeter_pos {param.find(':')};
        if(delimeter_pos == std::string::npos) throw runtime_error(string{"bencode deserialization error: wrong string format on argument "} + string{param});
        std::string len_str {param.substr(0, delimeter_pos)};
        size_t param_len {stoi(len_str)}; 
        string raw_prop{param.substr(delimeter_pos + 1, param_len)};
        if(param_len != raw_prop.length()) throw runtime_error("bencode deserialization error: wrong string format");
        return bencodeElem(raw_prop);
    }
    case bencodeKeySymbols::intstart : {
        if(param[0] != 'i') throw runtime_error("bencode deserialization error, wrong int format");
        size_t end_pos {param.find("e")};
        int result {stoi(string{param.substr(1,end_pos - 1)})}; // i _ _ _ e
        return bencodeElem{result};
    }
    case bencodeKeySymbols::liststart : {
        throw runtime_error{string{"error on attempt to use deserialize_simple on bencode container type - list"}};
        break;
    }
    case bencodeKeySymbols::mapstart : {
        throw runtime_error{string{"error on attempt to use deserialize_simple on bencode container type - dictionary"}};
        break;
    }
    }
}
bencodeElem TorrentClient::deserialize(const std::string_view &param) {
    switch (getKeyFromChar(param[0]))
    {
    case bencodeKeySymbols::stringstart : {
        return deserialize_simple(param);
    }
    case bencodeKeySymbols::intstart : {
        return deserialize_simple(param);
    }
    case bencodeKeySymbols::liststart : {
        size_t current_indent = 1;
        if(param[0] != 'l') throw runtime_error("bencode deserialization error, wrong list format");
        bencodeElem result(bencodeList{});
        bencodeList &ref = get<bencodeList>(*result.data);
        
        while (param[current_indent] != 'e' && current_indent <= param.length()) {
            ref.push_back(deserialize(param.substr(current_indent)));

            current_indent += ref.back().calculateStringSize();
        };
        return result;
    }
    case bencodeKeySymbols::mapstart : {
        size_t current_indent = 1;
        bencodeElem result(bencodeDict{});
        bencodeDict &ref = get<bencodeDict>(*result.data);
        string key{""};
        pair<string, bencodeElem> my_pair{};
        do
        {
            if (getKeyFromChar(param[current_indent]) != bencodeKeySymbols::stringstart) throw runtime_error("wrong bencode dictionary format: no string key");
            bencodeElem &&key_elem = deserialize(param.substr(current_indent));
            key = get<string>(*key_elem.data);
            current_indent += key_elem.calculateStringSize();
            my_pair.first = key;

            bencodeElem &&value_elem = deserialize(param.substr(current_indent));
            my_pair.second = value_elem;
            current_indent += value_elem.calculateStringSize();
            ref.insert(my_pair);
        } while (getKeyFromChar(param[current_indent]) != bencodeKeySymbols::end);
        return result;
    }
    default:
        throw runtime_error(string{"wrong bencode format: misplaced symbol "} + param[0]);
    break;
    }
}
inline bencodeKeySymbols TorrentClient::getKeyFromChar(char param)
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
        if(isdigit(static_cast<unsigned char>(param))) return bencodeKeySymbols::stringstart;
    }
    throw std::runtime_error(string{"getKeyFromChar error: unknown character '"} + param + "'");
}