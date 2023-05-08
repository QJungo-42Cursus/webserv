#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

std::vector<std::string> split(const std::string &str, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}