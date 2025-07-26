#include <parser/readers/data_reader.hpp>

using namespace TorrentClient;

class SimpleDataReader : DataReader<>
{
protected:
    std::ifstream input;
    bool is_good() const override {
    return input.good();
}
private:
public:

};