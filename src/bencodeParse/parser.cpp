#include <algorithm>
#include <cctype>
#include <fstream>
#include <iosfwd>
#include <parser/bencodeparse.hpp>
#include <string>

#include "../src/bencodeParse/parser_aux.cpp"

using namespace std;
using namespace TorrentClient;

struct streamState {
  ifstream &stream;
  streampos initialPos;
  ios_base::iostate initialFlag;
  bool canceled{false};

  streamState(ifstream &_input)
      : stream(_input), initialPos(_input.tellg()),
        initialFlag(_input.rdstate()) {}
  ~streamState() {
    if (canceled)
      return;
    stream.clear();
    stream.seekg(initialPos);
    stream.setstate(initialFlag);
  }
  inline void switch_state() noexcept { canceled = (canceled) ? false : true; }
};

iparser::iparser() = default;
/// @brief constructs iparser and with opened file
/// @param filePath path to file
iparser::~iparser() { input.~unique_ptr(); }
void iparser::runFileChecks() const {
  if (!filesystem::exists(input->UsedFilePath())) {
    throw runtime_error(input->UsedFilePath().u8string() +
                        string(" does not exist"));
  }

  if (!input->is_open())
    throw std::runtime_error(input->UsedFilePath().u8string() +
                             " file is not open");

  string extension = input->UsedFilePath().extension().string();
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);
  if (extension != ".torrent") {
    throw std::runtime_error(
        (input->UsedFilePath().u8string() + " not a .torrent file"));
  }

  if (std::filesystem::file_size(input->UsedFilePath()) > (512 * 1024))
    throw std::runtime_error(input->UsedFilePath().string() + " file too big");

  auto firstChar = input->operator[](0);
  std::string errorMsg = "Invalid torrent file: expected 'd', got '";
  if (!firstChar.has_value())
    errorMsg += "EOF";
  else if (firstChar.value() != 'd') {
    errorMsg += static_cast<char>(firstChar.value());
    errorMsg += "'";
    throw std::runtime_error(errorMsg);
  }
}

std::optional<streampos>
iparser::getPropertyPosition(const string_view &param) {
  if (param.empty())
    return nullopt;
  if (param.size() > chunkSize)
    throw runtime_error(string("search parameter cant be longer than ") +
                        to_string(chunkSize));
  if (!input->is_open())
    throw runtime_error("getPropertyPosition error, " +
                            input->UsedFilePath().string() += " is not open");

  unsigned int totalStringSize = param.size() + chunkSize;
  size_t currentPos = 0;
  auto read = [&currentPos,
               this]() -> std::optional<std::vector<unsigned char>> {
    auto block = input->get_block(currentPos, chunkSize);
    currentPos += (block.has_value()) ? block.value().size() : 0;
    return block;
  };

  std::optional<std::vector<unsigned char>> chunk{std::vector<unsigned char>{}};
  chunk.value().reserve(totalStringSize);
  std::fill_n(chunk.value().front(), param.size(), '0');
  vector<unsigned char> overlapBuffer{};
  overlapBuffer.reserve(param.size());
  auto begin = chunk.value().begin();
  auto end = chunk.value().end();
  _Vector_iterator<_Vector_val<std::_Simple_types<unsigned char>>> posIter;

  chunk = read();
  // data that you get when reading a chunkSize of bytes from file
  std::optional<std::vector<unsigned char>> data{};
  while (input->is_good()) {
    if (!chunk.has_value())
      return nullopt;

    unsigned int data_size = data.value().size();
    copy(
        data.value().begin(),
        (data.value().begin() + static_cast<unsigned int>(data.value().size())),
        (begin + static_cast<unsigned int>(param.size())));

    posIter = std::search(begin, end, param.begin(), param.end());
    if (posIter != end) {
      return ((currentPos - chunk.value().size() -
               static_cast<streamoff>(param.size())) +
              (posIter - begin));
    }

    copy(end - param.size(), end, overlapBuffer.begin());
    copy(overlapBuffer.begin(), overlapBuffer.begin() + param.size(),
         chunk.value().begin());

    data = read();
  }
  if (data.value().size() > 0) {

    begin = chunk.value().begin();
    end = begin + static_cast<unsigned int>(param.size()) +
          static_cast<unsigned int>(data.value().size());

    copy(data.value().data(), (data.value().data() + chunk.value().size()),
         (begin + static_cast<unsigned int>(param.size())));
    copy(overlapBuffer.begin(),
         overlapBuffer.begin() + static_cast<unsigned int>(param.size()),
         chunk.value().begin());

    posIter = std::search(begin, end, param.begin(), param.end());
    if (posIter != end) {
      return ((currentPos - chunk.value().size() -
               static_cast<streamoff>(param.size())) +
              (posIter - begin));
    }
  }
  return nullopt;
}

bencodeElem TorrentClient::deserialize_simple(const std::string_view &param) {
  size_t current_indent = 1;
  switch (getKeyFromChar(param[0])) {
  case bencodeKeySymbols::stringstart: {
    return bencodeElem{string{process_bencode_string(param).second}};
  }
  case bencodeKeySymbols::intstart: {
    if (param[0] != 'i')
      throw runtime_error("bencode deserialization error, wrong int format");
    if (param[1] == '0')
      throw runtime_error("bencode deserialization error, invalid int format");
    size_t end_pos{param.find("e")};
    int result{stoi(string{param.substr(1, end_pos - 1)})}; // i _ _ _ e
    return bencodeElem{result};
  }
  case bencodeKeySymbols::liststart: {
    throw runtime_error{
        string{"error on attempt to use deserialize_simple on bencode "
               "container type - list"}};
    break;
  }
  case bencodeKeySymbols::mapstart: {
    throw runtime_error{
        string{"error on attempt to use deserialize_simple on bencode "
               "container type - dictionary"}};
    break;
  }
  default:
    return bencodeElem{};
  }
}
bencodeElem TorrentClient::deserialize(const std::string_view &param) {
  switch (getKeyFromChar(param[0])) {
  case bencodeKeySymbols::stringstart: {
    return deserialize_simple(param);
  }
  case bencodeKeySymbols::intstart: {
    return deserialize_simple(param);
  }
  case bencodeKeySymbols::liststart: {
    size_t current_indent = 1;
    if (param[0] != 'l')
      throw runtime_error("bencode deserialization error, wrong list format");
    bencodeElem result(bencodeList{});
    auto &ref = get<bencodeList>(*result.data);

    while (param[current_indent] != 'e' && current_indent <= param.length()) {
      ref.push_back(deserialize(param.substr(current_indent)));

      current_indent += ref.back().calculateStringSize();
    };
    return result;
  }
  case bencodeKeySymbols::mapstart: {
    size_t current_indent = 1;
    bencodeElem result(bencodeDict{});
    auto &ref = get<bencodeDict>(*result.data);
    string key{""};
    pair<string, bencodeElem> my_pair{};
    do {
      if (getKeyFromChar(param[current_indent]) !=
          bencodeKeySymbols::stringstart)
        throw runtime_error("wrong bencode dictionary format: no string key");
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
    throw runtime_error(string{"wrong bencode format: misplaced symbol "} +
                        param[0]);
    break;
  }
}
inline bencodeKeySymbols TorrentClient::getKeyFromChar(char param) {
  switch (param) {
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
    if (isdigit(static_cast<unsigned char>(param)))
      return bencodeKeySymbols::stringstart;
  }
  throw std::runtime_error(string{"getKeyFromChar error: unknown character '"} +
                           param + "'");
}
