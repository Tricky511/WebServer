#pragma once

#include <assert.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <sys/socket.h> //recv() send()
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class Buffer
{
  private:
    std::vector<char> buffer;
    std::atomic<size_t> head, tail;
    size_t remain();

  public:
    Buffer(int initSize = 128);
    ~Buffer() = default;
    void init();
    std::string tostr();
    size_t getLen();
    void updhead(size_t len);
    void updtail(size_t len);
    void allocate(size_t len);
    void ensure(size_t len);
    void append(const std::string &str);
    void append(const char *str, size_t len);
    ssize_t readFd(int fd, int *error);
    ssize_t writeFd(int fd, int *error);
    const char *gethead() const;
    const char *gettail() const;
};
