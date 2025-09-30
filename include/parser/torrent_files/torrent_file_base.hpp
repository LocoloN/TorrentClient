#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace TorrentClient {
class info {
private:
public:
  info() noexcept = delete;
  info(const info &param) noexcept = default;
  info(info &&param) noexcept = default;
  info &operator=(const info &param) noexcept = default;
  info &operator=(info &&param) noexcept = default;

  virtual ~info() = default;
  virtual long unsigned int piece_length() const = 0;
  virtual std::string pieces() const = 0;
  virtual std::string name() const = 0;
  virtual std::map<long int, std::filesystem::path> files() const = 0;
};

class torrentFile {
protected:
  std::string passkey;
  unsigned int _amountOfUrls;
  std::filesystem::path _file_path;

private:
public:
  torrentFile() noexcept = delete;
  torrentFile(const torrentFile &param) noexcept = default;
  torrentFile(torrentFile &&param) noexcept = default;
  torrentFile &operator=(const torrentFile &param) noexcept = default;
  torrentFile &operator=(torrentFile &&param) noexcept = default;

  virtual ~torrentFile() = 0;

  inline std::filesystem::path file_path() const { return _file_path; };
  virtual std::shared_ptr<info> info_get() const = 0;
  virtual std::vector<std::string> announce() const = 0;
  virtual std::string comment() const = 0;
  virtual std::string created_by() const = 0;
  virtual std::time_t creation_date() const = 0;
};

} // namespace TorrentClient
