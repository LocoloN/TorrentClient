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
bencodeElem::bencodeElem(const std::vector<bencodeElem> &param) {
    data = (std::vector<bencodeElem>)param;
}
bencodeElem::bencodeElem(const std::map<std::string, bencodeElem> &param){
    data = param;
}

bencodeDataType& bencodeElem::stringInit()
{
    data = std::string();
    return data;
}
bencodeDataType& bencodeElem::intInit() {
    data = int();
    return data;
}
bencodeDataType& bencodeElem::vectorInit() {
    data = std::vector<bencodeElem>();
    return data;
}
bencodeDataType& bencodeElem::mapInit() {
    data = std::map<std::string, bencodeElem>();
    return data;
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
    switch (getStoredTypeAsKey(param))
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
template<typename T>
bencodeKeySymbols getKeyFromType();
template<>
bencodeKeySymbols getKeyFromType<std::string>(){
    return bencodeKeySymbols::stringstart;
}
template<>
bencodeKeySymbols getKeyFromType<int>(){
    return bencodeKeySymbols::intstart;
}
template<>
bencodeKeySymbols getKeyFromType<std::vector<bencodeElem>>(){
    return bencodeKeySymbols::liststart;
}
template<>
bencodeKeySymbols getKeyFromType<std::map<std::string, bencodeElem>>(){
    return bencodeKeySymbols::mapstart;
}

/// @brief returns bencodeKeySymbols value from one of bencodeDataType variant stored types
/// @param param used to get bencodeKeySymbol
/// @return bencodeKeySymbol from param
bencodeKeySymbols getStoredTypeAsKey(const bencodeElem& param){
    return std::visit([](auto &&arg) -> bencodeKeySymbols {
        return getKeyFromType<std::decay_t<decltype(arg)>>();
    }, param.data);
}

bencodeKeySymbols getKeyFromChar(const char &param)
{
    switch (param)
    {
    case 'i':
        return bencodeKeySymbols::intstart;
    break;
    case 'l':
        return bencodeKeySymbols::liststart;
    break;
    case 'd':
        return bencodeKeySymbols::mapstart;
    break;
    case 'e':
        return bencodeKeySymbols::end;
    break;
    default:
        if(isdigit(param)) return stringstart;
    break;
    }
    throw std::runtime_error("wrong char error");
}