#ifndef YAML_HELPER_H
#define YAML_HELPER_H

Option<std::string> find_key_value_line(std::string &str, const std::string &key, bool remove_line);

std::string get_value_from_line(const std::string &line);

Option<short> parse_methods(std::string &str);

std::string unpad_from_left(const std::string &str, size_t padding, bool throw_if_something);

#endif //YAML_HELPER_H
