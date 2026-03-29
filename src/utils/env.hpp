#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, std::string> loadEnv(
    const std::string& filename)
{
    std::unordered_map<std::string, std::string> env;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "[ENV.HPP] Cannot open: " << filename << "\n";
        return env;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t"));
            s.erase(s.find_last_not_of(" \t") + 1);
            };
        trim(key);
        trim(value);

        env[key] = value;
    }

    return env;
}