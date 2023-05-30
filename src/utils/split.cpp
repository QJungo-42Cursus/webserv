#include <vector>
#include <sstream>
#include <string>

std::vector<std::string> split(const std::string &str, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}

std::vector<std::string> split(const std::string &str, const std::string &delim)
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = str.length();
	size_t delim_len = delim.length();
	if (delim_len == 0)
		return elems;
	while (pos < len)
	{
		int find_pos = str.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(str.substr(pos, len - pos));
			break;
		}
		elems.push_back(str.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}