#pragma once
#include <bencodeparse.hpp>

using namespace std;
using namespace TorrentClient;

inline pair<size_t, string_view> process_bencode_string(const string_view &param) {
    pair<size_t, string_view> result;
    size_t delimeter_pos {param.find(':')};
    if(delimeter_pos == std::string::npos) throw runtime_error(string{"bencode deserialization error: wrong string format on argument "} + string{param});
    std::string len_str {param.substr(0, delimeter_pos)};
    result.first = stoull(len_str); 
    result.second = param.substr(delimeter_pos + 1, result.first);
    if(result.first != result.second.length()) throw runtime_error("bencode deserialization error: wrong string format");
    return result;
}