#include <string>
#include <vector>
#include <dirent.h>
#include <stdexcept>

/// This will return a string containing the HTML code for a directory listing
std::string dir_listing(std::string const &path)
{
	std::vector<std::string> files;
	files.push_back("./");
	files.push_back("../");

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) == NULL)
		throw std::runtime_error("Could not open directory");

	while ((ent = readdir(dir)) != NULL)
	{
		if (ent->d_name[0] != '.' && ent->d_name != "..")
			files.push_back(ent->d_name);
	}
	closedir(dir);

	std::string html =
			"<html><head><title>Index of " + path + "</title></head><body><h1>Index of " + path + "</h1><hr>\n";
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		html += "<a href=\"" + *it + "\">" + *it + "</a><br>\n";
	}
	html += "<hr></body></html>";
	return html;
}