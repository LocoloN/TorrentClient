#include <iostream>
#include "bencodeparse.hpp"
#include <filesystem>
using namespace std;

int main()
{
    //iparser obj(filesystem::path("../test/testfiles/Ultrakill.torrent"));
    bencodeElem elem{};
    cout << elem.data->index();
    return 0;
}
