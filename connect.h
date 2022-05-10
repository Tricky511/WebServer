#pragma once
#include "buffer.h"
#include "request.h"
#include "response.h"
#include <arpa/inet.h>
#include <sys/uio.h> //readv() writev()
class Connect
{
  private:
    int fd;

    bool isClose;
    sockaddr_in addr;
    Buffer wbuff, rbuff;
    Request request;
    Response response;
    int iovCnt;
    struct iovec iov[2];

  public:
    static const char *dir;
    static std::atomic<int> userCount;
    Connect();
    ~Connect();
    bool isKeepAlive() const
    {
        return request.isKeepAlive();
    }
    size_t writelen()
    {
        return iov[1].iov_len + iov[0].iov_len;
    }
    void init(int fd, const sockaddr_in &addr);
    bool handleConn();
    void closeConn();
    int getfd();
    ssize_t readToBuffer(int *error);
    ssize_t WriteFromBuffer(int *error);
};
