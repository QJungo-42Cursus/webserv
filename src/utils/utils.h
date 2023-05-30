#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

#include "Option.h"

std::vector<std::string> split(const std::string &str, char delim);

std::vector<std::string> split(const std::string &str, const std::string &delim);

std::string get_cwd();

#endif // UTILS_H
