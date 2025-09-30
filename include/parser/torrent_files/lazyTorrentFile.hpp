#pragma once

#include "parser/torrent_files/LazyProperty.hpp"
#include "parser/torrent_files/torrent_file_base.hpp"

namespace TorrentClient {

class lazyInfo : public info {
private:
  long unsigned int _piece_length;
  LazyProperty<std::string> _pieces;
  std::string _name;
  std::map<long int, std::filesystem::path> _files;

public:
  lazyInfo();
  long unsigned int piece_length() const override;
  std::string pieces() const override;
  std::string name() const override;
  std::map<long int, std::filesystem::path> files() const override;
};

class lazyTorrentFile : public torrentFile {
protected:
  std::shared_ptr<info> _info;
  std::vector<std::string> _announce;
  std::string _comment;
  std::string _created_by;
  std::time_t _creation_date;

public:
  lazyTorrentFile();
  ~lazyTorrentFile() override;
  std::shared_ptr<info> info_get() const noexcept override;
  std::vector<std::string> announce() const noexcept override;
  std::string comment() const noexcept override;
  std::string created_by() const noexcept override;
  std::time_t creation_date() const noexcept override;
};
} // namespace TorrentClient
