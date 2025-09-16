#pragma once
#include <filesystem>
#include <optional>
#include <vector>

namespace TorrentClient {
enum readerType { simple = 1 };

class DataReader {
protected:
  std::filesystem::path used_file_path{""}; // NOLINT

private:
public:
  DataReader() = default;
  virtual ~DataReader() = default;
  DataReader(const DataReader &param) noexcept = delete;
  DataReader(DataReader &&param) noexcept = default;
  DataReader &operator=(const DataReader &param) noexcept = delete;
  DataReader &operator=(DataReader &&param) noexcept = default;

  virtual bool is_good() const = 0;
  [[nodiscard]] virtual bool is_open() const noexcept = 0;
  virtual bool open_file(const std::filesystem::path &path) noexcept = 0;
  virtual void close_file() noexcept = 0;
  static std::unique_ptr<DataReader> create(readerType param);
  virtual std::optional<unsigned char>
  operator[](const std::streampos &index) const noexcept = 0;
  [[nodiscard]] virtual std::optional<std::vector<unsigned char>>
  get_block(std::streampos offset, std::size_t size) const noexcept = 0;
  std::filesystem::path UsedFilePath() { return used_file_path; }
};
} // namespace TorrentClient
