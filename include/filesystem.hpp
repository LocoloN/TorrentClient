#include <filesystem>
#include <vector>

class projFilesystemTests;

namespace TorrentClient {
    class projFilesystem 
    {
        std::vector<std::filesystem::path> programFolders  =
        {   "torrents",
            "temp"
        };
        
        friend class ::projFilesystemTests;
    public:
        projFilesystem();
        void init() const;
        bool check_folders_integrity() const noexcept;
        bool check_torrent_pairs() const noexcept;
    };
}