#pragma once
#include "connect.h"
#include "epoller.h"
#include "threadpool.h"
#include "timer.h"
#include <assert.h>
class Webserver
{
  private:
    int port, timeout, listenFd;
    char *dir;
    bool isClose;
    std::unique_ptr<TimeHeap> timer;
    std::unique_ptr<ThreadPool> threadpool;
    std::unique_ptr<Epoll> epoller;
    std::unordered_map<int, Connect> user;
    static const uint32_t listenEvent = EPOLLRDHUP | EPOLLET;
    static const uint32_t connectionEvent = EPOLLONESHOT | EPOLLRDHUP | EPOLLET;
    static const int MAX_FD = 65536;
    bool initSocket();
    int setFdNonblock(int fd);
    void addConn(int fd, sockaddr_in addr);
    void closeConn(Connect *client);
    void sendError(int fd, const char *info);
    void flushTime(Connect *client);
    void handleListen();
    void handleRead(Connect *client);
    void handleWrite(Connect *client);
    void onProcess(Connect *client);
    void onRead(Connect *client);
    void onWrite(Connect *client);

  public:
    Webserver(int port, int timeout, int threadNum, int queMaxSize);
    ~Webserver();
    void Start();
};
