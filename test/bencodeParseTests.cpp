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
        benchmark.mapInit();
        std::get<std::map<std::string, bencodeElem>>(benchmark.data)["property1"] = bencodeElem(228);
        std::get<std::map<std::string, bencodeElem>>(benchmark.data)["listtest"] = bencodeElem(std::vector<bencodeElem> {
            std::string("test"),
            (int)1337
        });

        testObj.openFile(fakeTorrentpath);
        testObj.parseToSequence();
        return (*(testObj.sequence.get()) == benchmark);
    }
    

};

parserTests helper;

TEST_CASE("Parser openfile isGoodOrOpen", "[parser][openfile]")
{
    helper.initialiseTestObj();
    REQUIRE(helper.isOpenOrGood() == true);
}
TEST_CASE("Parser openfile torrent format checks" ,"[parser][openfile]")
{
    helper.initialiseTestObj();
    REQUIRE(helper.isErrorOnNonTorrentExtension() == true);
    REQUIRE(helper.isErrorOnIncorrecTorrentFile() == true);
}
TEST_CASE("Parse test on fake torrent", "[parser][openfile]")
{
    helper.initialiseTestObj();
    REQUIRE(helper.FillFakeSequence() == true);
}
TEST_CASE("Get key from bencodeElem variant", "[parser][nonMember]")
{
    helper.initialiseTestObj();
    auto param = bencodeElem(static_cast<int>(123));
    REQUIRE(getStoredTypeAsKey(bencodeElem(static_cast<int>(123))) == bencodeKeySymbols::intstart);
    REQUIRE(getStoredTypeAsKey(bencodeElem(std::string("test"))) == bencodeKeySymbols::stringstart);
    REQUIRE(getStoredTypeAsKey(bencodeElem(std::vector<bencodeElem>())) == bencodeKeySymbols::liststart);
    REQUIRE(getStoredTypeAsKey(bencodeElem(std::map<std::string, bencodeElem>())) == bencodeKeySymbols::mapstart);
}

