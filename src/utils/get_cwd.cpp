#include <string>
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>

std::string get_cwd() {
    char *_pwd = getcwd(NULL, 0);
    if (_pwd == NULL)
        throw std::runtime_error("getcwd failed");
    std::string pwd = _pwd;
    free(_pwd);
    return pwd;
}