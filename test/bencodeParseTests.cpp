#include <catch2/catch_test_macros.hpp>
#include "bencodeparse.hpp"
#include <iostream>
#include <string>
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
        return getKeyFromChar(param);
    }
    bool getPropertyPosTest(const std::string &param) {
        testObj.openFile(realTorrentpath);
        testObj.runFileChecks();
        if(testObj.input.eof()){
            throw runtime_error(testObj.usedFilePath.string() += " EOF error");
        }
        if(testObj.input.fail()){
            throw runtime_error(testObj.usedFilePath.string() += " badbit or failbit error");
        }
        size_t pos = testObj.getPropertyPosition(param);
        testObj.input.seekg(pos);
        std::vector<char> result(param.size() + 5);
        testObj.input.read(result.data(), param.size());

        if(param == std::string(result.begin(), (result.begin() + param.size()))) return true;
        if(pos == -1) throw runtime_error(string("cant find property ") += param + string(" result is -1"));

        throw runtime_error("property position error, in file " + testObj.usedFilePath.string() +  
            string(" at position ") + to_string(pos) + string(" expected ") + param + " got " + string(result.data()));
            
        return false; 
    }
};

bool serialize_test(const bencodeElem &elem, string comp)
{
    string result = elem.serialize();
    if(result != comp) throw runtime_error("Wrong serialisation result, expected " + comp + " got " + result);
    return result == comp;
    return false;
}

bool calculateStringSize_test(const bencodeElem elem, size_t comp)
{
    size_t result(elem.calculateStringSize());
    REQUIRE(result == comp);
    if(result != comp)
    {
        throw runtime_error(string("calculateStringSize error, expected ") += to_string(comp) += string(" got ") += to_string(result));
    }
    return true;
}
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
    REQUIRE((bencodeElem(static_cast<int>(123))).getStoredTypeAsKey() == bencodeKeySymbols::intstart);
    REQUIRE((bencodeElem(std::string("test"))).getStoredTypeAsKey() == bencodeKeySymbols::stringstart);
    REQUIRE((bencodeElem(std::vector<bencodeElem>())).getStoredTypeAsKey() == bencodeKeySymbols::liststart);
    REQUIRE((bencodeElem(std::map<std::string, bencodeElem>())).getStoredTypeAsKey() == bencodeKeySymbols::mapstart);
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
    CHECK(helper.getPropertyPosTest("publisher-url"));
    CHECK(helper.getPropertyPosTest("beebo"));
    CHECK(helper.getPropertyPosTest("beebo2"));
    CHECK(helper.getPropertyPosTest("Textures20:marble-tile-warm"));
    CHECK(helper.getPropertyPosTest("created by"));
    CHECK(helper.getPropertyPosTest("/ann13:announce-listll23:http://"));
}
TEST_CASE("Serialize test","[bencodeElem][bencodeElemMember]")
{
    CHECK(serialize_test(bencodeElem("boba"), string("4:boba")));
    CHECK(serialize_test(bencodeElem(123), string("i123e")));
    CHECK(serialize_test(bencodeElem(112323), string("i112323e")));
    CHECK(serialize_test(bencodeElem(0), string("i0e")));
    CHECK(serialize_test(bencodeElem(
        bencodeDict
    {
        {string("announce"), bencodeElem("babaika")},
        {string("info"), bencodeList {bencodeElem(1337), bencodeElem("chepuha")}},
        {string("test"), bencodeElem(123)}
    }),
        string("d") +
        string("8:announce7:babaika") + 
        string("4:infoli1337e7:chepuhae") +
        string("4:testi123e") +
        string("e")));
    CHECK(serialize_test(bencodeElem(bencodeList{
        bencodeElem(123),
        bencodeElem("test_string"),
        bencodeElem(bencodeDict{{string("aboba"), bencodeElem(123)},{string("shershavchik"), bencodeElem("chto?")}})}),
        string("li123e11:test_stringd5:abobai123e12:shershavchik5:chto?ee")));
}
TEST_CASE("calculateStringSize test","[bencodeElem][bencodeElemMember]") 
{
    bencodeDict dict = bencodeDict {
        {string("announce"), bencodeElem("babaika")},
        {string("info"), bencodeList {bencodeElem(1337), bencodeElem("chepuha")}},
        {string("test"), bencodeElem(123)}
    };
    CHECK(calculateStringSize_test(bencodeElem(123), 5));
    CHECK(calculateStringSize_test(bencodeElem("boba"), 6));
    CHECK(calculateStringSize_test(bencodeElem(dict), 55));
    CHECK(calculateStringSize_test(bencodeElem(bencodeList{
        bencodeElem(123),
        bencodeElem("test_string"),
        bencodeElem(bencodeDict{{string("aboba"), bencodeElem(123)},{string("shershavchik"), bencodeElem("chto?")}})}), 57));
}
TEST_CASE("Deserialize test","[bencodeElem][bencodeElemMember]") 
{
    string i{"i123e"};
    string str{"11:string_test"};
    CHECK(deserialize(i) == bencodeElem(123));\
    CHECK(deserialize(str) == bencodeElem(string{"string_test"}));

}