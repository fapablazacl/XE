
#pragma once 

#include <string>
#include <vector>
#include <fstream>

inline std::string parent_path(const std::string &str) {
    return str.substr(0, str.find_last_of("/\\")) + "/";
}


inline std::string replace_all(const std::string &str, const std::string &search, const std::string &replace) {
    std::string result = str;
    
    size_t pos = 0;
    do {
        pos = result.find(search);
        
        if (pos != std::string::npos) {
            result.replace(pos, search.size(), replace);
        }
    }
    while (pos != std::string::npos);
    
    return result;
};


inline std::vector<std::string> split(const std::string &str, const std::string &delimiter) {
    std::string s = str;
    std::vector<std::string> tokens;
    
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        
        tokens.push_back(token);
        
        s.erase(0, pos + delimiter.length());
    }
    
    tokens.push_back(s);
    
    return tokens;
}


inline std::string join(const std::vector<std::string> &elements, const std::string &delimiter) {
    std::string str;
    
    for (size_t i = 0; i < elements.size(); i++) {
        if (elements[i].empty()) {
            continue;
        }
        
        str += elements[i];
        
        if (i < elements.size() - 1) {
            str += delimiter;
        }
    }
    
    return str;
}


inline std::string normalize_path(const std::string &str) {
    std::string result = str;
    
    std::replace(result.begin(), result.end(), '\\', '/');
    
    return result;
}


inline bool can_be_opened(const std::string &filePath) {
    std::fstream fs;
    
    fs.open(filePath.c_str(), std::ios::in);
    
    return fs.is_open();
}
