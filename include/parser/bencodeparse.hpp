#pragma once
#include "parser/torrent_files/torrent_file_base.hpp"
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <parser/readers/DataReader.hpp>
#include <string>
#include <variant>
#include <vector>

class iparserTests;

namespace TorrentClient {
class bencodeElem;
using bencodeList = std::vector<bencodeElem>;
/// @brief keys should be ordered lexicographicaly
using bencodeDict = std::map<std::string, bencodeElem>;
using bencodeDataType =
    std::variant<std::string, int, bencodeList, bencodeDict>;
/// @brief stringstart = 0, intstart = 1, liststart = 2, mapstart = 3, end = 4
enum bencodeKeySymbols {
  stringstart = 0, // <length of string>:<string itself>
  intstart = 1,    // i<integer>e
  liststart = 2,   // l<other bencode types>e
  mapstart = 3,    // d<bencode string><other bencode types>e keys should be
                   // ordered lexicographicaly
  end = 4
};
class bencodeElem {
public:
  std::unique_ptr<bencodeDataType> data;
  bencodeElem();
  bencodeElem(const bencodeElem &arg);
  bencodeElem(bencodeElem &&arg) noexcept;
  bencodeElem(const std::string &);
  bencodeElem(const int &);
  bencodeElem(const bencodeList &);
  bencodeElem(const bencodeDict &);
  ~bencodeElem() = default;
  /// @brief returns bencodeKeySymbols value from one of bencodeDataType variant
  /// stored types
  /// @param param used to get bencodeKeySymbol
  /// @return bencodeKeySymbol from param
  [[nodiscard]] bencodeKeySymbols getStoredTypeAsKey() const;
  /// @brief serialize elem
  /// @return string in bencode format
  [[nodiscard]] std::string serialize() const;
  /// @brief calculates serialized string length without serializing
  //
  [[nodiscard]] size_t calculateStringSize() const;
  bencodeElem &operator=(const bencodeElem &);
  bencodeElem &operator=(bencodeElem &&) noexcept;
  bool operator==(const bencodeElem &) const;
  bool operator!=(const bencodeElem &) const;
};
/// @brief serializes string to bencode format
/// @param param string to serialize
/// @return prepends length of string and symbol ':' according to bencode format
std::string serialize(const std::string_view &param);
/// @brief serializes int to bencode format
/// @param param int to serialize
/// @return string in format 'i' + param + 'e'
std::string serialize(const int &param);
/// @brief used to recursively turn any type from bencode format string to
/// bencodeElem
/// @exception runtime_error if cant deserialise
/// @param param string that starts with one of bencode types
/// @return bencodeElem
bencodeElem deserialize(const std::string_view &param);
/// @brief used to deserialize simple bencode types - int and string
/// @param param string starting with simple type
/// @exception runtime_error on attempt to use this function on bencode
/// container type - list or dict
/// @return bencodeElem with int or string type
bencodeElem deserialize_simple(const std::string_view &param);

enum TorrentFiletype {
  empty = 0,
  lazy = 1,
};

class iparser {
private:
  friend class ::iparserTests;
  static constexpr int chunkSize = 4096;

public:
  iparser() = default;
  iparser(std::unique_ptr<DataReader> reader) noexcept;
  iparser(const iparser &parser) = delete;
  iparser(iparser &&param) noexcept;
  virtual ~iparser();
  std::unique_ptr<DataReader> input{};

  inline constexpr int get_chunkSize() const;
  /// @brief used to construct bencodeElem from string
  /// @param param string_view that represents one of bencode types
  /// @return bencodeElem
  std::shared_ptr<torrentFile> get_torrent(TorrentFiletype file_type);
  /// @brief used to retrieve property position from opened bencode format file
  /// \note sets stream position to returned index
  /// @param param property name
  /// @exception runtime_error when reading error could not find the property
  /// @return index of first character of property in file - 8:announce will
  /// return index of character '8'
  /// @return
  std::optional<std::streampos>
  getPropertyPosition(const std::string_view &param);

  /// @brief checks opened file for .torrent extension and if it is suited for
  /// reading
  /// @exception runtime_error if not a .torrent file, file too big, file is not
  /// open
  /// @return true if everything is ok
  void runFileChecks() const;
  [[nodiscard]] inline bool is_open() const noexcept {
    return input->is_open();
  }
  /// @brief sets openedFilePath property and initializes std::ifstream input
  /// needed for opening file and checking it .torrent format
  /// @param path path to .torrent file
  /// @return true if everything is ok
  iparser &operator=(const iparser &param) noexcept = delete;
  iparser &operator=(iparser &&param) noexcept;
};

class oparser;

bencodeKeySymbols getKeyFromChar(char param);
} // namespace TorrentClient
