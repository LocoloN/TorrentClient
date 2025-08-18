#pragma once

#include <fstream>
#include <parser/readers/DataReader.hpp>

using namespace std;
using namespace TorrentClient;

class SimpleDataReader_tests;
// class SimpleDataReader;

namespace TorrentClient {
class SimpleDataReader : virtual public DataReader {
protected:
  mutable ifstream input; // NOLINT
  friend class ::SimpleDataReader_tests;

private:
public:
  SimpleDataReader() = default;
  ~SimpleDataReader() override = default;
  SimpleDataReader(const SimpleDataReader &param) noexcept = delete;
  SimpleDataReader(SimpleDataReader &&param) noexcept;
  SimpleDataReader &operator=(const SimpleDataReader &param) noexcept = delete;
  SimpleDataReader &operator=(SimpleDataReader &&param) noexcept;

  inline bool is_good() const override { return input.good(); };
  inline streamsize gcount() const noexcept;
  bool is_open() const noexcept override { return input.is_open(); }
  bool open_file(const std::filesystem::path &path) noexcept override;
  void close_file() noexcept override;
  optional<unsigned char>
  operator[](const std::streampos &index) const noexcept override;
  optional<vector<unsigned char>>
  get_block(streampos offset, std::size_t size) const noexcept override;
};
} // namespace TorrentClient
