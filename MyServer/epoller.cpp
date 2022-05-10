
#include "epoller.h"
#include <iostream>
Epoll::Epoll(int maxEvents) : epollFd(epoll_create(64)), events(maxEvents)
{
}
Epoll::~Epoll()
{
    close(epollFd);
}
bool Epoll::add(int fd, uint32_t events)
{
    if (fd < 0)
        return false;
    struct epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == 0;
}
bool Epoll::mod(int fd, uint32_t events)
{
    if (fd < 0)
        return false;
    struct epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) == 0;
}
bool Epoll::del(int fd)
{
    if (fd < 0)
        return false;
    struct epoll_event ev = {0};
    return epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &ev) == 0;
}
int Epoll::wait(int time)
{
    return epoll_wait(epollFd, &events[0], static_cast<int>(events.size()), time);
}
int Epoll::getFd(int i)
{
    return events[i].data.fd;
}
uint32_t Epoll::getEvents(int i)
{
    return events[i].events;
}