#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <map>
#include <string>

class HttpResponse {
public:
    HttpResponse(int status_code, const std::string& reason_phrase,
                 const std::map<std::string, std::string>& headers, const std::string& body);

    std::string to_string() const;
    void set_body(const std::string& body);

private:
    int status_code_;
    std::string reason_phrase_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};

#endif
