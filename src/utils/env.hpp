// It's just a utility tool that manually parses each line in an env file 
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
		std::cerr << "[ENV.HPP] This " << filename << " cannot be open\n";
		return env; 
	}

	std::string line; 
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#') continue; 

		size_t position = line.find('=');
		if (position == std::string::npos) continue; 

		std::string key = line.substr(0, position);
		std::string value = line.substr(position + 1);

		env[key] = value;
	}

	return env; 
}