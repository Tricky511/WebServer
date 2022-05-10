#pragma once

#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "buffer.h"

class Request
{

  public:
    enum STATE
    {
        LINE,
        HEADER,
        BODY,
        FINISH
    };
    void init();
    Request()
    {
        init();
    };
    ~Request() = default;
    std::string getpath() const;
    std::string getmethod() const;
    std::string getversion() const;
    std::string getbody() const;
    bool isKeepAlive() const;
    bool parse(Buffer &buff);

  private:
    bool parseRequestLine(const std::string &line);
    void parseRequestHeader(const std::string &line);
    void parseRequestBody(const std::string &line);
    void parsePath();
    void parsePost();
    int convertHex(char c);
    STATE state;
    std::string path, method, version, body;
    std::unordered_map<std::string, std::string> header;
    std::unordered_map<std::string, std::string> post;
};
