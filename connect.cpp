#include "connect.h"
const char *Connect::dir;
std::atomic<int> Connect::userCount;
Connect::Connect()
{
    fd = -1;
    addr = {0};
    isClose = true;
}
Connect::~Connect()
{
    closeConn();
}
void Connect::init(int fd, const sockaddr_in &addr)
{
    userCount++;
    this->addr = addr;
    this->fd = fd;
    wbuff.init(), rbuff.init();
    isClose = false;
}
bool Connect::handleConn()
{
    request.init();
    if (rbuff.getLen() <= 0)
        return false;
    if (request.parse(rbuff))
        response.init(dir, request.getpath(), request.isKeepAlive(), 200);
    else
        response.init(dir, request.getpath(), false, 400);
    response.makeResponse(wbuff);
    iov[0].iov_base = const_cast<char *>(wbuff.gethead());
    iov[0].iov_len = wbuff.getLen();
    iovCnt = 1;

    if (response.fileLen() > 0 && response.file())
    {
        iov[1].iov_base = response.file();
        iov[1].iov_len = response.fileLen();
        iovCnt = 2;
    }
    return true;
}
void Connect::closeConn()
{
    response.unmap();
    if (isClose == false)
    {
        isClose = true;
        userCount--;
        close(fd);
    }
}
ssize_t Connect::readToBuffer(int *error)
{
    ssize_t len = -1;
    while (true)
    {
        len = rbuff.readFd(fd, error);
        if (len <= 0)
            break;
    }

    return len;
}
ssize_t Connect::WriteFromBuffer(int *error)
{
    ssize_t len = -1;
    do
    {
        len = writev(fd, iov, iovCnt);
        if (len <= 0)
        {
            *error = errno;
            break;
        }
        if (iov[0].iov_len + iov[1].iov_len == 0)
        {
            break;
        }
        else if (static_cast<size_t>(len) > iov[0].iov_len)
        {
            iov[1].iov_base = (uint8_t *)iov[1].iov_base + (len - iov[0].iov_len);
            iov[1].iov_len -= (len - iov[0].iov_len);
            if (iov[0].iov_len)
            {
                wbuff.init();
                iov[0].iov_len = 0;
            }
        }
        else
        {
            iov[0].iov_base = (uint8_t *)iov[0].iov_base + len;
            iov[0].iov_len -= len;
            wbuff.updhead(len);
        }
    } while (wbuff.getLen() > 10240);
    return len;
}
int Connect::getfd()
{
    return fd;
}