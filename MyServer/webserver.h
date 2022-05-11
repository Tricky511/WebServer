#pragma once
#include "connect.h"
#include "epoller.h"
#include "threadpool.h"
#include "timer.h"
#include <assert.h>
class Webserver
{
  private:
    int port, listenFd;
    static int timeout;
    char *dir;
    bool isClose;
    static std::unique_ptr<TimeHeap> timer;
    static std::unique_ptr<ThreadPool> threadpool;
    static std::unique_ptr<Epoll> epoller;
    std::vector<Connect> user;
    static const uint32_t listenEvent = EPOLLRDHUP | EPOLLET;
    static const uint32_t connectionEvent = EPOLLONESHOT | EPOLLRDHUP | EPOLLET;
    static const int MAX_FD = 65536;
    bool initSocket();
    int setFdNonblock(int fd);
    void addConn(int fd, sockaddr_in addr);
    static void closeConn(Connect *client);
    void sendError(int fd, const char *info);
    static void flushTime(Connect *client);
    void handleListen();
    static void handleRead(Connect *client);
    static void handleWrite(Connect *client);
    static void onProcess(Connect *client);
    static void onRead(Connect *client);
    static void onWrite(Connect *client);

  public:
    Webserver(int port, int timeout, int threadNum, int queMaxSize);
    ~Webserver();
    void Start();
};
