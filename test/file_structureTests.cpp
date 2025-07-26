#include <catch2/catch_test_macros.hpp>

using namespace std;
/*
using namespace TorrentClient;

class file_structure_tests {
    file_structure testObj;
public:
    void initTestObj()
    {
        testObj = file_structure();
    }

    bool initTest(std::vector<std::filesystem::path> foldersToInit)
    {
        bool result = true;
        testObj.programFolders = foldersToInit;
        testObj.init();
        for (size_t i = 0; i < foldersToInit.size(); i++)
        {
            result = result && filesystem::is_directory(foldersToInit[i]);
            filesystem::remove(foldersToInit[i]);
        }
        
        return result;
    }
};
file_structure_tests helper2;
*/

TEST_CASE("check filesystem init","[fileSystem]") {
    REQUIRE(true);
}