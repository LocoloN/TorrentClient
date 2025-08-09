#include <fstream>
#include <parser/readers/SimpleDataReader.hpp>

using namespace std;
using namespace TorrentClient;

bool SimpleDataReader::reading_checks() const noexcept {
  return input.good() && !input.eof();
}
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
SimpleDataReader::operator[](const size_t &index) const noexcept {
  if (!reading_checks())
    return nullopt;
  if (0 > index || index > filesystem::file_size(used_file_path))
    return nullopt;
  input.seekg(index);
  return input.peek();
}

optional<vector<unsigned char>>
SimpleDataReader::get_block(size_t offset, size_t size) const noexcept {
  if (!reading_checks())
    return nullopt;
  if (static_cast<std::streampos>(offset + size) >
      filesystem::file_size(used_file_path))
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
