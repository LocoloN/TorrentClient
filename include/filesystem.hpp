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
        bool checkFoldersIntegrity() const;
        bool checkTorrentPairs() const;
    };
}