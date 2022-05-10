#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unordered_map>

#include "buffer.h"

class Response
{
  private:
    std::string getFileType();
    void errorBody(Buffer &buff, std::string str);
    void makeResponseLine(Buffer &buff);
    void makeResponseHeader(Buffer &buff);
    void makeResponseBody(Buffer &buff);
    int code;
    bool isKeepAlive;
    char *mfile;
    std::string dir, path;
    struct stat mfileStat;
    static const std::unordered_map<std::string, std::string> TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;

  public:
    Response();
    ~Response();
    char *file();
    size_t fileLen() const;
    void unmap();
    void init(const std::string dir, const std::string path, bool isKeepAlive, int code);
    void makeResponse(Buffer &buff);
};