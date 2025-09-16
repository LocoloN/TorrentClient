#include "parser/bencodeparse.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <variant>

using namespace std;
using namespace TorrentClient;

bencodeElem *fakeSequence;

class iparserTests {
public:
  iparser testObj;

  iparserTests() : testObj(DataReader::create(readerType::simple)) {}

  const std::filesystem::path txtpath = "../test/testfiles/testfile.txt";
  const std::filesystem::path wrongFormatpath =
      "../test/testfiles/wrongformat.torrent";
  const std::filesystem::path fakeTorrentpath =
      "../test/testfiles/fake.torrent";
  const std::filesystem::path realTorrentpath =
      "../test/testfiles/Ultrakill.torrent";

  bool fakeTorrentCanBeRead(const std::filesystem::path &torrentPath) {
    testObj.input->open_file(fakeTorrentpath);
    testObj.input->is_good();
    return true;
  }
  inline bencodeKeySymbols getKeyFromCharTest(const char &param) {
    return getKeyFromChar(param);
  }
  bool getPropertyPosTest(std::string param) {
    bool isopen{false};
    isopen = testObj.input->is_open();
    REQUIRE(isopen);
    INFO(string{"Opened file is "} + testObj.input->UsedFilePath().string());
    INFO(string{"param = "} + param);
    try {
      testObj.runFileChecks();
    } catch (const std::exception &e) {
      FAIL("Exception: " << e.what());
    }
    INFO("runFileChecks successful");

    std::optional<std::streampos> pos;
    INFO("calling getPropertyPosition()");
    pos = testObj.getPropertyPosition(param);
    INFO("getPropertyPosition finished without exceptions");

    REQUIRE(pos.has_value());

    auto result = testObj.input->get_block(pos.value(), param.size());
    REQUIRE(result.has_value());

    if ((string{result.value().begin(),
                (result.value().begin() +
                 static_cast<long long>(param.size()))} == param)) {
      return true;
    }
    FAIL(("property position error, in file " +
          testObj.input->UsedFilePath().string() + string(" at position ") +
          to_string(pos.value()) + string(" expected ") + param + " got " +
          string(result.value().begin(), result.value().end())));
    return false;
  }
};

bool serialize_test(const bencodeElem &elem, string comp) {
  string result = elem.serialize();
  if (result != comp) {
    FAIL(("Wrong serialisation result, expected " + comp + " got " + result));
  }
  return result == comp;
  return false;
}

bool calculateStringSize_test(const bencodeElem elem, size_t comp) {
  size_t result(elem.calculateStringSize());
  REQUIRE(result == comp);
  if (result != comp) {
    throw runtime_error(string("calculateStringSize error, expected ") +=
                        to_string(comp) += string(" got ") +=
                        to_string(result));
  }
  return true;
}
bool deserialize_test(const string_view &param, const bencodeElem &comp) {
  bencodeElem &&test_elem{deserialize(param)};

  if (test_elem.getStoredTypeAsKey() != comp.getStoredTypeAsKey()) {
    throw runtime_error{"Type mismatch: expected " +
                        to_string(comp.getStoredTypeAsKey()) + " got " +
                        to_string(test_elem.getStoredTypeAsKey())};
  }

  switch (test_elem.getStoredTypeAsKey()) {
  case bencodeKeySymbols::stringstart: {
    auto comp_val = get_if<string>(comp.data.get());
    auto test_val = get_if<string>(test_elem.data.get());

    if (!comp_val)
      throw runtime_error{
          string{"Deserialisation error: stringstart comp_val is nullptr"}};
    if (!test_val)
      throw runtime_error{
          string{"Deserialisation error: stringstart test_val is nullptr"}};

    if (test_elem != comp)
      throw runtime_error{string{"wrong deserialization:  expected "} +
                          *comp_val + string{" got "} + *test_val};
    return true;
  }
  case bencodeKeySymbols::intstart: {
    auto test_val = get_if<int>(test_elem.data.get());
    auto comp_val = get_if<int>(comp.data.get());

    if (!test_val)
      throw runtime_error{
          string{"Deserialisation error: intstart test_val is nullptr"}};
    if (!comp_val)
      throw runtime_error{
          string{"Deserialisation error: intstart comp_val is nullptr"}};

    if (test_elem != comp)
      throw runtime_error{string{"wrong deserialization:  expected "} +
                          to_string(*comp_val) + string{" got "} +
                          to_string(*test_val)};
    return true;
  }
  case bencodeKeySymbols::liststart: {
    auto &test_elem_ref = get<bencodeList>(*test_elem.data);

    if (test_elem.getStoredTypeAsKey() != comp.getStoredTypeAsKey())
      throw runtime_error{
          string{"Error in deserialize_test: deserialized stored type is not "
                 "the same as in comp: expected "} +
          to_string(comp.getStoredTypeAsKey()) + string{"got"} +
          to_string(test_elem.getStoredTypeAsKey())};

    if (test_elem != comp)
      throw runtime_error{string{"wrong deserialization: list"}};
    size_t i = 0;
    for (auto &&j : get<bencodeList>(*comp.data)) {
      if (test_elem_ref[i] != j)
        throw runtime_error{string{"wrong deserialization: expected "} +
                            to_string(get<int>(*comp.data)) + string{" got "} +
                            to_string(get<int>(*test_elem.data))};
      i++;
    }
    return true;
  }
  case bencodeKeySymbols::mapstart: {
    auto &test_elem_ref = get<bencodeDict>(*test_elem.data);
    for (const auto &ipair : test_elem_ref) {
      for (const auto &jpair : get<bencodeDict>(*comp.data)) {
        if (ipair.first != jpair.first)
          throw runtime_error{string{"wrong deserialization: expected key "} +
                              jpair.first + string{" got "} + ipair.first};
        if (ipair.second != jpair.second)
          throw runtime_error{string{"wrong deserialization: expected "} +
                              jpair.second.serialize() + string{" got "} +
                              ipair.second.serialize()};
      }
    }
    return true;
  }
  default:
    return false;
    break;
  }
  return true;
}

iparserTests helper;

TEST_CASE_METHOD(iparserTests, "runFileChecks_test", "[parser][member]") {
  INFO("Opening file...");
  testObj.input->open_file(realTorrentpath);
  INFO("Performing file checks (runFileChecks)");
  REQUIRE_NOTHROW(testObj.runFileChecks());
}

TEST_CASE("Get key from bencodeElem variant", "[parser][nonMember]") {

  REQUIRE((bencodeElem(int{123})).getStoredTypeAsKey() ==
          bencodeKeySymbols::intstart);
  REQUIRE((bencodeElem(std::string("test"))).getStoredTypeAsKey() ==
          bencodeKeySymbols::stringstart);
  REQUIRE((bencodeElem(std::vector<bencodeElem>())).getStoredTypeAsKey() ==
          bencodeKeySymbols::liststart);
  REQUIRE((bencodeElem(std::map<std::string, bencodeElem>()))
              .getStoredTypeAsKey() == bencodeKeySymbols::mapstart);
}
TEST_CASE_METHOD(iparserTests, "Get key from char test",
                 "[parser][nonMember]") {
  REQUIRE(getKeyFromCharTest('i') == bencodeKeySymbols::intstart);
  REQUIRE(getKeyFromCharTest('l') == bencodeKeySymbols::liststart);
  REQUIRE(getKeyFromCharTest('d') == bencodeKeySymbols::mapstart);
  REQUIRE(getKeyFromCharTest('e') == bencodeKeySymbols::end);
  CHECK(getKeyFromCharTest('0') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('1') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('2') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('3') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('4') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('5') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('6') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('7') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('8') == bencodeKeySymbols::stringstart);
  CHECK(getKeyFromCharTest('9') == bencodeKeySymbols::stringstart);
}
TEST_CASE_METHOD(iparserTests, "Test files exist", "[nonMember]") {
  INFO("CWD: " + std::filesystem::current_path().string());
  REQUIRE(std::filesystem::exists(this->fakeTorrentpath));
  REQUIRE(std::filesystem::exists(this->realTorrentpath));
  REQUIRE(std::filesystem::exists(this->txtpath));
  REQUIRE(std::filesystem::exists(this->wrongFormatpath));
}

TEST_CASE_METHOD(iparserTests, "Get property position test",
                 "[parser][member]") {
  INFO("Opening file...");
  REQUIRE(testObj.input->open_file(realTorrentpath));
  INFO("File opened");
  auto vals = GENERATE("announce", "publisher-url", "beebo", "beebo2",
                       "Textures20:marble-tile-warm", "created by",
                       "/ann13:announce-listll23:http://");
  INFO("Calling getPropertyPosTest()");
  REQUIRE(getPropertyPosTest(vals));
  // vector<string> vals = {"announce",
  //                        "publisher-url",
  //                        "beebo",
  //                        "beebo2",
  //                        "Textures20:marble-tile-warm",
  //                        "created by",
  //                        "/ann13:announce-listll23:http://"};
  // for (auto item : vals) {
  //   CHECK(getPropertyPosTest(item));
  // }
  //
  //  CHECK(getPropertyPosTest("announce"));
  // CHECK(getPropertyPosTest("publisher-url"));
  // CHECK(getPropertyPosTest("beebo"));
  // CHECK(getPropertyPosTest("beebo2"));
  // CHECK(getPropertyPosTest("Textures20:marble-tile-warm"));
  // CHECK(getPropertyPosTest("created by"));
  // CHECK(getPropertyPosTest("/ann13:announce-listll23:http://"));
}

TEST_CASE("Serialize test", "[bencodeElem][bencodeElemMember]") {
  CHECK(serialize_test(bencodeElem("boba"), string("4:boba")));
  CHECK(serialize_test(bencodeElem(123), string("i123e")));
  CHECK(serialize_test(bencodeElem(112323), string("i112323e")));
  CHECK(serialize_test(bencodeElem(0), string("i0e")));
  CHECK(serialize_test(
      bencodeElem(
          bencodeDict{{string("announce"), bencodeElem("babaika")},
                      {string("info"),
                       bencodeList{bencodeElem(1337), bencodeElem("chepuha")}},
                      {string("test"), bencodeElem(123)}}),
      string("d") + string("8:announce7:babaika") +
          string("4:infoli1337e7:chepuhae") + string("4:testi123e") +
          string("e")));
  CHECK(serialize_test(
      bencodeElem(
          bencodeList{bencodeElem(123), bencodeElem("test_string"),
                      bencodeElem(bencodeDict{
                          {string("aboba"), bencodeElem(123)},
                          {string("shershavchik"), bencodeElem("chto?")}})}),
      string("li123e11:test_stringd5:abobai123e12:shershavchik5:chto?ee")));
}
TEST_CASE("calculateStringSize test", "[bencodeElem][bencodeElemMember]") {
  CHECK(calculateStringSize_test(
      bencodeElem{bencodeDict{{string{"proverka"}, bencodeElem{1234}}}}, 18));
  CHECK(calculateStringSize_test(bencodeElem{bencodeList{bencodeDict{
                                     {string{"proverka"}, bencodeElem{1234}}}}},
                                 20));
  bencodeDict dict = bencodeDict{
      {string("announce"), bencodeElem("babaika")},
      {string("info"), bencodeList{bencodeElem(1337), bencodeElem("chepuha")}},
      {string("test"), bencodeElem(123)}};
  CHECK(calculateStringSize_test(bencodeElem(123), 5));
  CHECK(calculateStringSize_test(bencodeElem("boba"), 6));
  CHECK(calculateStringSize_test(bencodeElem(dict), 55));
  CHECK(calculateStringSize_test(
      bencodeElem{
          bencodeList{bencodeElem{123}, bencodeElem{string{"proverka"}}}},
      17));
  CHECK(calculateStringSize_test(
      bencodeElem(
          bencodeList{bencodeElem(123), bencodeElem("test_string"),
                      bencodeElem(bencodeDict{
                          {string("aboba"), bencodeElem(123)},
                          {string("shershavchik"), bencodeElem("chto?")}})}),
      57));
}
TEST_CASE("Deserialize test", "[bencodeElem][nonMember]") {
  CHECK(deserialize_test(string{"i1234e"}, bencodeElem{int{1234}}));
  CHECK(
      deserialize_test(string{"8:proverka"}, bencodeElem{string("proverka")}));

  bencodeElem comp{
      bencodeList{bencodeElem{1234}, bencodeElem{string{"proverka"}}}};
  CHECK(deserialize_test(string{"li1234e8:proverkae"}, comp));

  CHECK(deserialize_test(
      string{"li1234el8:proverkai123ee4:jopae"},
      bencodeElem{
          bencodeList{bencodeElem{1234},
                      bencodeElem{bencodeList{bencodeElem{string{"proverka"}},
                                              bencodeElem{123}}},
                      bencodeElem(string{"jopa"})}}));
  CHECK(deserialize_test(
      string{"d8:proverkai1234ee"},
      bencodeElem{bencodeDict{{string{"proverka"}, bencodeElem{1234}}}}));
  CHECK(deserialize_test(
      string("li123e11:test_stringd5:abobai123e12:shershavchik5:chto?ee"),
      bencodeElem(
          bencodeList{bencodeElem(123), bencodeElem("test_string"),
                      bencodeElem(bencodeDict{
                          {string("aboba"), bencodeElem(123)},
                          {string("shershavchik"), bencodeElem("chto?")}})})));
}
