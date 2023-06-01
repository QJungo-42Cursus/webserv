#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <map>

class HttpResponse {
public:
    HttpResponse();

    void set_version(const std::string& version);
    void set_status(int status_code, const std::string& status_description);
    void add_header(const std::string& header_name, const std::string& header_value);
    void set_body(const std::string& body);
   
    std::string to_string() const;

private:
    std::string _version;
    int _status_code;
    std::string _status_description;
    std::map<std::string, std::string> _headers;
    std::string _body;
};

#endif // HTTP_RESPONSE_H

