#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <string>

class HttpRequest {
public:
    HttpRequest(const std::string& raw_request);

    int get_method() const;
    std::string get_path() const;
    std::string get_version() const;
    std::map<std::string, std::string> get_headers() const;
    std::string get_body() const;
	void log() const;

private:
    int method_;
    std::string path_;
    std::string version_;
    std::map<std::string, std::string> headers_;
    std::string body_;

    void parse_request(const std::string& raw_request);
    void parse_request_line(const std::string& line);
    void parse_header(const std::string& line);
};

#endif
