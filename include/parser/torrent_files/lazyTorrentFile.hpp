#pragma once

#include "parser/torrent_files/torrent_file_base.hpp"

namespace TorrentClient {

class lazyInfo : public info {
private:
public:
  lazyInfo();
  virtual long unsigned int piece_length() const;
  virtual LazyProperty<std::string> pieces() const;
  virtual std::string name() const;
  virtual std::map<long int, std::filesystem::path> files() const;
};

class lazyTorrentFile : public torrentFile {
protected:
  size_t infoPosition;

public:
  lazyTorrentFile();
  ~lazyTorrentFile();
  std::shared_ptr<info> info_get() const noexcept override;
  std::vector<std::string> announce() const noexcept override;
  std::string comment() const noexcept override;
  std::string created_by() const noexcept override;
  std::time_t creation_date() const noexcept override;
};
} // namespace TorrentClient
