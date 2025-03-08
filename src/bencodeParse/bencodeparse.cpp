#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <exception>
#include "bencodeparse.hpp"
#include <memory>
#include <type_traits>

bencodeElem::bencodeElem()
{
    data = bencodeDataType();
}
bencodeElem::bencodeElem(const std::string &param) {
    data = param;
}
bencodeElem::bencodeElem(const int &param) {
    data = param;
}
bencodeElem::bencodeElem(const bencodeList &param) {
    data = (std::vector<bencodeElem>)param;
}
bencodeElem::bencodeElem(const bencodeDict &param){
    data = param;
}
void bencodeElem::operator= (const bencodeElem& param)
{
    this->data = param.data;
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
    switch (parser::getStoredTypeAsKey(param))
    {
        case intstart:
            return std::get<int>(this->data) == std::get<int>(param.data);
        break;
        case stringstart:
            return 0 == std::get<std::string>(this->data).compare(std::get<std::string>(param.data));
        break;
        case liststart:
            return std::get<std::vector<bencodeElem>>(param.data) == std::get<std::vector<bencodeElem>>(this->data);
        break;
        case mapstart:        
            return compareMaps<std::map<std::string, bencodeElem>>(std::get<std::map<std::string, bencodeElem>>(this->data), 
            std::get<std::map<std::string, bencodeElem>>(param.data));
        break;
    
    default:
        return false;
    break;
    }
}