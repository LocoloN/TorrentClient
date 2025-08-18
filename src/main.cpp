#include "parser/bencodeparse.hpp"
#include <filesystem>
#include <parser/readers/DataReader.hpp>

using namespace std;
using namespace TorrentClient;
int main() {
  iparser parser{DataReader::create(readerType::simple)};

  parser.input->open_file(std::filesystem::path{
      R"(E:\Projects\TorrentClient\test\testfiles\Ultrakill.torrent)"});
  parser.getPropertyPosition("info");
  return 0;
}
