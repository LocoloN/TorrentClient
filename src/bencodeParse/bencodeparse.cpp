#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <exception>
#include "bencodeparse.hpp"
#include <memory>
#include <type_traits>
#include <cmath>

using namespace std;

bencodeElem::bencodeElem() : data(make_unique<bencodeDataType>()) { }
bencodeElem::bencodeElem(const bencodeElem &arg) : data(make_unique<bencodeDataType>(*arg.data)) { }
bencodeElem::bencodeElem(bencodeElem &&arg) : data(std::move(arg.data)) { }
bencodeElem::bencodeElem(const std::string &arg) : data(make_unique<bencodeDataType>(arg)) { }
bencodeElem::bencodeElem(const int &arg) : data(make_unique<bencodeDataType>(arg)) { }
bencodeElem::bencodeElem(const bencodeList &arg) : data(make_unique<bencodeDataType>(arg)) { };
bencodeElem::bencodeElem(const bencodeDict &arg) : data(make_unique<bencodeDataType>(arg)) { }
size_t bencodeElem::calculateStringSize() const {
    switch (data->index())
    {
        case bencodeKeySymbols::stringstart : 
        {
            auto len = get<string>(*data).length();
            return floor(log10(len) + 2) + len;
        }
        case bencodeKeySymbols::intstart :
                return floor(log10(get<int>(*data)) + 1) + 2;
        break;
        case bencodeKeySymbols::liststart : {
            bencodeList &mylist = get<bencodeList>(*data);
            size_t resultSize(2);
            for (auto const &item : mylist)
            {
                resultSize += item.calculateStringSize();
            }
            return resultSize;
        }
        case bencodeKeySymbols::mapstart : 
        {
            bencodeDict &myMap = get<bencodeDict>(*data);
            size_t resultSize(2);
            for (auto const &[key, val] : myMap)
            {
                resultSize += bencodeElem(key).calculateStringSize();
                resultSize += val.calculateStringSize();
            }
            return resultSize;
        }
        case bencodeKeySymbols::end :
            return size_t(-1);
        break;
    }
}
std::string bencodeElem::serialize() const
{
    switch (data->index())
    {
    case bencodeKeySymbols::intstart : 
        return serialize_to_bencode(get<int>(*data));
    break;
    case bencodeKeySymbols::stringstart : {
        return serialize_to_bencode(get<string>(*data));
    }
    case bencodeKeySymbols::liststart : {
        bencodeList &blist = get<bencodeList>(*data);
        string result;
        result.reserve(calculateStringSize());
        result.append("l");
        for (auto &&i : blist)
        {
            result.append(i.serialize());
        }
        result.append("e");
        return result;
    }

    case bencodeKeySymbols::mapstart : {
        bencodeDict dict = get<bencodeDict>(*data);
        string result;
        result.reserve(calculateStringSize());
        result.append("d");
        for (auto const&[key, val] : dict)
        {
            result.append(serialize_to_bencode(key));
            result.append(val.serialize());
        }
        result.append("e");         
        return result;
    }
    default:
    return string("");
    break;
    return string("");
    }
}
void bencodeElem::operator= (const bencodeElem& arg) {
    *data = *arg.data;
}
void bencodeElem::operator=(bencodeElem && arg) {
    data = move(arg.data);
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
std::string serialize_to_bencode(const std::string_view &param) {
     return to_string(param.length()).append(":").append(param);
}
std::string serialize_to_bencode(const int &param) {
    return string("i") + to_string(param) + "e";
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
bencodeKeySymbols bencodeElem::getStoredTypeAsKey() const {
    return std::visit([](auto &&arg) -> bencodeKeySymbols {
        return getKeyFromType<std::decay_t<decltype(arg)>>();
    }, *this->data);
}