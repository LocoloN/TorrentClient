#include "file_structure.hpp"
#include <filesystem>

using namespace std;
using namespace TorrentClient;

void file_structure::init() const {
  for (const auto &programFolder : programFolders) {
    filesystem::create_directory(filesystem::path(programFolder));
  }
}

bool file_structure::check_folders_integrity() const noexcept {
  bool result = true;
  for (const auto &item : programFolders) {
    result = result && filesystem::is_directory(item);
  }

  return result;
}

// bool projFilesystem::checkTorrentPairs() const
// {
//     throw new runtime_error("FUNCTION NOT YET IMPLEMENTED");
//     return false;
// }
