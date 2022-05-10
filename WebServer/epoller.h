#pragma once
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
class Epoll
{
  private:
    int epollFd;
    std::vector<struct epoll_event> events;

  public:
    Epoll(int maxEvents = 1024);
    ~Epoll();
    bool add(int fd, uint32_t events);
    bool mod(int fd, uint32_t events);
    bool del(int fd);
    int wait(int time = -1);
    int getFd(int i);
    uint32_t getEvents(int i);
};