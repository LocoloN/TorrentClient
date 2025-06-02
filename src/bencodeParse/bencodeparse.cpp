#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <exception>
#include "bencodeparse.hpp"
#include <memory>
#include <type_traits>

using namespace std;

bencodeElem::bencodeElem()
{
    data = make_unique<bencodeDataType>();
}
bencodeElem::bencodeElem(const bencodeElem &arg) {
    *data = *arg.data;
}
bencodeElem::bencodeElem(bencodeElem &&arg) {
    data = std::move(arg.data);
}
bencodeElem::bencodeElem(const std::string &arg) {
    data = make_unique<bencodeDataType>(arg);
}
bencodeElem::bencodeElem(const int &arg) {
    data = make_unique<bencodeDataType>(arg);
}
bencodeElem::bencodeElem(const bencodeList &arg) {
    data = make_unique<bencodeDataType>(arg);
}
bencodeElem::bencodeElem(const bencodeDict &arg){
    data = make_unique<bencodeDataType>(arg);
}
void bencodeElem::operator= (const bencodeElem& arg) {
    *data = *arg.data;
}
void bencodeElem::operator=(bencodeElem && arg) {
    data = move(arg.data);
}
template <typename Map>
bool compareMaps (Map const &lhs, Map const &rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}
bool bencodeElem::operator!= (const bencodeElem& param) const
{
    return !(this->data == param.data);
}
bool bencodeElem::operator==(const bencodeElem& param) const
{
    switch (param.getStoredTypeAsKey())
    {
        case intstart:
            return std::get<int>(*data) == std::get<int>(*param.data);
        break;
        case stringstart:
            return 0 == std::get<std::string>(*data).compare(std::get<std::string>(*param.data));
        break;
        case liststart:
            return std::get<std::vector<bencodeElem>>(*param.data) == std::get<std::vector<bencodeElem>>(*data);
        break;
        case mapstart:        
            return compareMaps<std::map<std::string, bencodeElem>>(std::get<std::map<std::string, bencodeElem>>(*data), 
            std::get<std::map<std::string, bencodeElem>>(*param.data));
        break;
    
    default:
        return false;
    break;
    }
}