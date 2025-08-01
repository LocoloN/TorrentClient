#include <parser/readers/DataReader.hpp>
#include <parser/readers/SimpleDataReader.hpp>

using namespace TorrentClient;
using namespace std;

unique_ptr<DataReader> DataReader::create(readerType param) { 
    switch (param) {
        case readerType::simple : {
            return std::unique_ptr<DataReader>(new SimpleDataReader());
        }
        default : {
            return nullptr;
        }
    }
}
