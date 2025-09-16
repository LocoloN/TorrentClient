#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <parser/readers/SimpleDataReader.hpp>

using namespace std;
using namespace TorrentClient;

using Catch::Matchers::Equals;

class SimpleDataReader_tests {
protected:
  unique_ptr<DataReader> testObj;

  const std::filesystem::path txtpath = "../test/testfiles/testfile.txt";
  const std::filesystem::path wrongFormatpath =
      "../test/testfiles/wrongformat.torrent";
  const std::filesystem::path fakeTorrentpath =
      "../test/testfiles/fake.torrent";
  const std::filesystem::path realTorrentpath =
      "..\\test\\testfiles\\Ultrakill.torrent";

public:
  SimpleDataReader_tests() { testObj = DataReader::create(readerType::simple); }
  SimpleDataReader_tests(const std::filesystem::path &path) {
    testObj = DataReader::create(readerType::simple);
    testObj->open_file(path);
  }
};

TEST_CASE_METHOD(SimpleDataReader_tests, "SimpleDataReader::operator[]_test",
                 "[member][SimpleDataReader][get_data]") {
  testObj->open_file(txtpath);
  REQUIRE(testObj->operator[](2) == 'x');
  REQUIRE(testObj->operator[](3) == 't');
  REQUIRE(testObj->operator[](5) == '0');
}
TEST_CASE_METHOD(SimpleDataReader_tests, "SimpleDataReader::get_block_test",
                 "[member][SimpleDataReader][get_data]") {
  REQUIRE(testObj->open_file(realTorrentpath));
  auto result = testObj->get_block(0, 1024);
  REQUIRE(result.has_value());
  string compare{
      "d8:announce23:http://bt3.t-ru.org/ann13:announce-listll23:http://"
      "bt3.t-ru.org/annel31:http://retracker.local/"
      "announceee7:comment51:https://rutracker.org/forum/"
      "viewtopic.php?t=636137410:created by18:qBittorrent v4.6.313:creation "
      "datei1713266762e4:infod5:filesld6:lengthi625662820e4:pathl9:ULTRAKILL14:"
      "ULTRAKILL_Data15:StreamingAssets2:aa19:StandaloneWindows6420:music_"
      "assets_.bundleeed6:lengthi798e4:pathl5:Steam9:steamapps23:appmanifest_"
      "1229490.acfeed6:lengthi2240e4:pathl9:ULTRAKILL23:license "
      "information.txteed6:lengthi650752e4:pathl9:ULTRAKILL13:ULTRAKILL."
      "exeeed6:lengthi1077896e4:pathl9:ULTRAKILL23:UnityCrashHandler64.exeeed6:"
      "lengthi26173064e4:pathl9:ULTRAKILL15:UnityPlayer.dlleed6:lengthi33360e4:"
      "pathl9:ULTRAKILL22:WinPixEventRuntime.dlleed6:lengthi808e4:pathl9:"
      "ULTRAKILL10:Cybergrind8:Patterns13:BigBridge.cgpeed6:lengthi975e4:"
      "pathl9:ULTRAKILL10:Cybergrind8:Patterns10:Cliffs.cgpeed6:lengthi592e4:"
      "pathl9:ULTRAKILL10:Cybergrind8:Patterns12:LowCover.cgpeed6:lengthi668e4:"
      "pathl9:ULTRAKILL10:Cybergrind8:Patterns7:Pit.cgp"};
  CHECK(std::equal(result.value().begin(), result.value().end(),
                   compare.begin()));
}
