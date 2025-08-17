#include <fstream>
#include <parser/readers/SimpleDataReader.hpp>

using namespace std;
using namespace TorrentClient;

bool SimpleDataReader::open_file(const std::filesystem::path &path) noexcept {
  if (!filesystem::exists(path)) {
    input.close();
    input.clear();
    used_file_path = "";
    return false;
  }
  input.clear();
  input.open(path, std::ios_base::binary);
  used_file_path = path;

  if (!input.is_open()) {
    input.close();
    input.clear();
    used_file_path = "";
    return false;
  }
  return true;
}

optional<unsigned char>
SimpleDataReader::operator[](const streampos &index) const noexcept {
  if (!is_good())
    return nullopt;
  if (0 > index || index > filesystem::file_size(used_file_path))
    return nullopt;
  input.seekg(index);
  return input.peek();
}

SimpleDataReader &
SimpleDataReader::operator=(SimpleDataReader &&param) noexcept {
  input = std::move(param.input);
  used_file_path = param.used_file_path;
  return *this;
}

optional<vector<unsigned char>>
SimpleDataReader::get_block(streampos offset, streampos size) const noexcept {
  if (!is_good())
    return nullopt;
  if (offset + size > filesystem::file_size(used_file_path))
    return nullopt;
  optional<vector<unsigned char>> result{vector<unsigned char>{}};
  result.value().reserve(filesystem::file_size(used_file_path));
  input.seekg(offset);
  input.read(reinterpret_cast<char *>(result.value().data()), size);
  return result;
}

void SimpleDataReader::close_file() noexcept {
  input.clear();
  input.close();
  used_file_path = "";
}

bool SimpleDataReader::is_good() const { return input.good(); }

SimpleDataReader::SimpleDataReader(SimpleDataReader &&param) noexcept
    : input(std::move(param.input)) {
  used_file_path = param.used_file_path;
}

inline streamsize SimpleDataReader::gcount() const noexcept {
  return input.gcount();
}
