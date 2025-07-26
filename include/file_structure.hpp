#include <filesystem>
#include <vector>

class file_structure_tests;

namespace TorrentClient {
    
    class file_structure 
    {
        std::vector<std::filesystem::path> programFolders  =
        {   "torrents",
            "temp"
        };
        
        friend class ::file_structure_tests;
    public:
        file_structure();
        void init() const;
        bool check_folders_integrity() const noexcept;
        bool check_torrent_pairs() const noexcept;
    };
}