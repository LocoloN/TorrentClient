#include <catch2/catch_test_macros.hpp>
#include "bencodeparse.hpp"
#include <iostream>
#include <map>
#include <variant>
const std::filesystem::path txtpath = "../test/testfiles/testfile.txt";
const std::filesystem::path wrongFormatpath = "../test/testfiles/wrongformat.torrent";
const std::filesystem::path fakeTorrentpath = "../test/testfiles/fake.torrent";
bencodeElem *fakeSequence;

class parserTests
{
public:
    parser testObj;
    inline void initialiseTestObj()
    {
        testObj = parser();
    }
    
    bool isOpenOrGood()
    {
        testObj.openFile(txtpath);
        bool result = false;
        result = testObj.input.is_open();
        result = (result || testObj.input.good());
        return result;
    }
    bool isErrorOnNonTorrentExtension()
    {
        bool result = false;
        result = !testObj.openFile(txtpath);
        
        return result;
    }
    //checks if .torrent dont start with 'd'
    bool isErrorOnIncorrecTorrentFile()
    {
        bool result = false;
        result = !testObj.openFile(wrongFormatpath);
        
        return result;
    }
    bool FillFakeSequence()
    { 
        bencodeElem benchmark;
        benchmark = bencodeElem(bencodeDict());
        std::get<std::map<std::string, bencodeElem>>(benchmark.data)["property1"] = bencodeElem(228);
        std::get<std::map<std::string, bencodeElem>>(benchmark.data)["listtest"] = bencodeElem(std::vector<bencodeElem> {
            std::string("test"),
            (int)1337
        });

        testObj.openFile(fakeTorrentpath);
        testObj.parseToTree();
        return (*(testObj.parseToTree().get()) == benchmark);
    }
    inline bencodeKeySymbols getKeyFromCharTest(const char & param)
    {
        return testObj.getKeyFromChar(param);
    }
};

parserTests helper;

TEST_CASE("Parser openfile isGoodOrOpen", "[parser][openfile][member]")
{
    helper.initialiseTestObj();
    REQUIRE(helper.isOpenOrGood() == true);
}
TEST_CASE("Parser openfile torrent format checks" ,"[parser][openfile][member]")
{
    helper.initialiseTestObj();
    REQUIRE(helper.isErrorOnNonTorrentExtension() == true);
    REQUIRE(helper.isErrorOnIncorrecTorrentFile() == true);
}
// TEST_CASE("Parse test on fake torrent", "[parser][openfile][member]")
// {
//     helper.initialiseTestObj();
//     REQUIRE(helper.FillFakeSequence() == true);
// }
TEST_CASE("Get key from bencodeElem variant", "[parser][nonMember]")
{
    helper.initialiseTestObj();
    auto param = bencodeElem(static_cast<int>(123));
    REQUIRE(parser::getStoredTypeAsKey(bencodeElem(static_cast<int>(123))) == bencodeKeySymbols::intstart);
    REQUIRE(parser::getStoredTypeAsKey(bencodeElem(std::string("test"))) == bencodeKeySymbols::stringstart);
    REQUIRE(parser::getStoredTypeAsKey(bencodeElem(std::vector<bencodeElem>())) == bencodeKeySymbols::liststart);
    REQUIRE(parser::getStoredTypeAsKey(bencodeElem(std::map<std::string, bencodeElem>())) == bencodeKeySymbols::mapstart);
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

TEST_CASE("Parse to element test","[parser][member]")
{
    int temp = parser::bencodeToType<int>("i243819e");
    REQUIRE(temp == 243819);
    
    REQUIRE(std::string("testcasebiba").compare(parser::bencodeToType<std::string>("12:testcasebiba")) == 0);
}