#pragma once

#include <string>
#include <vector>

namespace common {

typedef std::vector<std::string> Tokens;
inline void split(const std::string & src, const std::string & sep, std::vector<std::string> & tokens) {
    tokens.clear();
    std::string s;
    for(auto ch : src) {
        if (sep.find(ch) != std::string::npos) {
            tokens.push_back(s);
            s = "";
        } else {
            s += ch;
        }
    }
    if ( s.length() || tokens.size() ) {
        tokens.push_back(s);
    }
}

}  // namespace common

