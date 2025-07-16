#include "filesystem.hpp"
#include <filesystem>
#include <exception>

using namespace std;
using namespace TorrentClient;

projFilesystem::projFilesystem() {
int x = 0;
}

void projFilesystem::init() const  {
    for (size_t i = 0; i < programFolders.size(); i++)
    {
        filesystem::create_directory(filesystem::path(programFolders[i]));
    }
}

bool projFilesystem::check_folders_integrity() const noexcept
{
    bool result = true;
    for (const auto &item : programFolders)
    {
        result = result && filesystem::is_directory(item);
    }
    
    return result;
}

// bool projFilesystem::checkTorrentPairs() const
// {
//     throw new runtime_error("FUNCTION NOT YET IMPLEMENTED");
//     return false;
// }
