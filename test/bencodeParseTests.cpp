#include <catch2/catch_test_macros.hpp>
#include "bencodeparse.hpp"
#include <iostream>
#include <map>
#include <variant>

using namespace std;
const std::filesystem::path txtpath = "../test/testfiles/testfile.txt";
const std::filesystem::path wrongFormatpath = "../test/testfiles/wrongformat.torrent";
const std::filesystem::path fakeTorrentpath = "../test/testfiles/fake.torrent";
const std::filesystem::path realTorrentpath = "..\\test\\testfiles\\Ultrakill.torrent";
bencodeElem *fakeSequence;

class iparserTests
{
public:
    iparser testObj;
    inline void initialiseTestObj()
    {
        testObj = iparser();
    }    
    bool TorrentFileChecksTest(const std::filesystem::path &torrentPath) {        
        testObj.openFile(torrentPath);
        testObj.runFileChecks();
        if (!testObj.input.good()) {
            throw runtime_error(torrentPath.string() += string(" ifstream error, iostate = ") += to_string(testObj.input.rdstate()));
        }
        if (!testObj.readingChecks()) {
            throw runtime_error(torrentPath.string() += string(" readingChecks() error, iostate = ") += to_string(testObj.input.rdstate()));
        }
        return true;
    }
    bool fakeTorrentCanBeRead(const std::filesystem::path &torrentPath) {
        testObj.openFile(torrentPath);
        testObj.readingChecks();
    }
    inline bencodeKeySymbols getKeyFromCharTest(const char & param)
    {
        return testObj.getKeyFromChar(param);
    }
    size_t getPropertyPosTest(const std::string_view &param) {
        testObj.openFile(realTorrentpath);
        testObj.runFileChecks();
        if(testObj.input.eof()){
            throw runtime_error(testObj.usedFilePath.string() += " EOF error");
        }
        if(testObj.input.fail()){
            throw runtime_error(testObj.usedFilePath.string() += " badbit or failbit error");
        }
        
        return testObj.getPropertyPosition(param);
    }
};

iparserTests helper;

TEST_CASE("torrent can be opened and read", "[parser][openfile][member]")
{
    helper.initialiseTestObj();
    CHECK(helper.TorrentFileChecksTest(fakeTorrentpath));
    helper.initialiseTestObj();
    CHECK(helper.TorrentFileChecksTest(realTorrentpath));
}
TEST_CASE("Get key from bencodeElem variant", "[parser][nonMember]")
{
    helper.initialiseTestObj();
    auto param = bencodeElem(static_cast<int>(123));
    REQUIRE(iparser::getStoredTypeAsKey(bencodeElem(static_cast<int>(123))) == bencodeKeySymbols::intstart);
    REQUIRE(iparser::getStoredTypeAsKey(bencodeElem(std::string("test"))) == bencodeKeySymbols::stringstart);
    REQUIRE(iparser::getStoredTypeAsKey(bencodeElem(std::vector<bencodeElem>())) == bencodeKeySymbols::liststart);
    REQUIRE(iparser::getStoredTypeAsKey(bencodeElem(std::map<std::string, bencodeElem>())) == bencodeKeySymbols::mapstart);
}
TEST_CASE("Get key from char test","[parser][nonMember]")
{
    helper.initialiseTestObj();
    REQUIRE(helper.getKeyFromCharTest('i') == bencodeKeySymbols::intstart);
    REQUIRE(helper.getKeyFromCharTest('l') == bencodeKeySymbols::liststart);
    REQUIRE(helper.getKeyFromCharTest('d') == bencodeKeySymbols::mapstart);
    REQUIRE(helper.getKeyFromCharTest('e') == bencodeKeySymbols::end);
    CHECK(helper.getKeyFromCharTest('0') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('1') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('2') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('3') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('4') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('5') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('6') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('7') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('8') == bencodeKeySymbols::stringstart);
    CHECK(helper.getKeyFromCharTest('9') == bencodeKeySymbols::stringstart);
}
TEST_CASE("Get property position test","[parser][member]")
{
    helper.initialiseTestObj();
    CHECK(helper.getPropertyPosTest("announce") == 2);
    CHECK(helper.getPropertyPosTest("announce-list") == 38);
    CHECK(helper.getPropertyPosTest("comment") == 120);
    CHECK(helper.getPropertyPosTest("info") == 245);
    CHECK(helper.getPropertyPosTest("length") == 1337);
}