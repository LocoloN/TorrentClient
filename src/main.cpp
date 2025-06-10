#include <iostream>
#include "bencodeparse.hpp"
#include <filesystem>
using namespace std;

int main()
{
    //iparser obj(filesystem::path("../test/testfiles/Ultrakill.torrent"));
    iparser obj(filesystem::path("../test/testfiles/fake.torrent"));
    auto pos = obj.getPropertyPosition("listtest");
    cout << pos;

    return 0;
}
