#include <catch2/catch_test_macros.hpp>
#include "filesystem.hpp"

using namespace std;
using namespace TorrentClient;

class projFilesystemTests {
    projFilesystem testObj;
public:
    void initTestObj()
    {
        testObj = projFilesystem();
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

projFilesystemTests helper2;
TEST_CASE("check filesystem init","[fileSystem]") {
    helper2.initTestObj();

    REQUIRE(helper2.initTest({
        "beeb",
        "bob",
        "le mamant"
    }));
};