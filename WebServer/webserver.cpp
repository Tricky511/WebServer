#include "webserver.h"

Webserver::Webserver(int port, int timeout, int threadNum, int queMaxSize)
    : port(port), timeout(timeout), isClose(false), timer(new TimeHeap()),
      threadpool(new ThreadPool(threadNum, queMaxSize)), epoller(new Epoll())
{
    dir = getcwd(nullptr, 256);
    strncat(dir, "/resources/", 16);
    Connect::userCount = 0;
    Connect::dir = dir;
    if (!initSocket())
    {
        std::cout << "Init Fail" << std::endl;
        isClose = true;
    }
}
Webserver::~Webserver()
{
    close(listenFd);
    isClose = true;
    free(dir);
}
bool Webserver::initSocket()
{
    int ret;
    struct sockaddr_in addr;
    if (port > 65535 || port < 1024)
        return false;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
        return false;
    }

    int optval = 1;
    ret = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
    if (ret == -1)
    {
        close(listenFd);
        return false;
    }

    ret = bind(listenFd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        close(listenFd);
        return false;
    }

    ret = listen(listenFd, 6);
    if (ret < 0)
    {
        close(listenFd);
        return false;
    }

    ret = epoller->add(listenFd, listenEvent | EPOLLIN);
    if (ret == 0)
    {
        close(listenFd);
        return false;
    }
    setFdNonblock(listenFd);
    return true;
}
int Webserver::setFdNonblock(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}
void Webserver::addConn(int fd, sockaddr_in addr)
{
    user[fd].init(fd, addr);
    if (timeout > 0)
    {
        timer->add(fd, timeout, std::bind(&Webserver::closeConn, this, &user[fd]));
    }
    epoller->add(fd, connectionEvent | EPOLLIN);
    setFdNonblock(fd);
}
void Webserver::closeConn(Connect *client)
{
    epoller->del(client->getfd());
    client->closeConn();
}
void Webserver::sendError(int fd, const char *info)
{
    send(fd, info, strlen(info), 0);
    close(fd);
}
void Webserver::flushTime(Connect *client)
{
    if (timeout > 0)
        timer->upd(client->getfd(), timeout);
}
void Webserver::handleListen()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    while (true)
    {
        int fd = accept(listenFd, (struct sockaddr *)&addr, &len);
        if (fd <= 0)
            return;
        else if (Connect::userCount >= MAX_FD)
        {
            sendError(fd, "Server busy!");
            return;
        }
        addConn(fd, addr);
    }
}
void Webserver::handleRead(Connect *client)
{
    flushTime(client);

    threadpool->Submit(std::bind(&Webserver::onRead, this, client));
}
void Webserver::handleWrite(Connect *client)
{
    flushTime(client);

    threadpool->Submit(std::bind(&Webserver::onWrite, this, client));
}
void Webserver::onRead(Connect *client)
{
    int ret = -1;
    int readErrno = 0;
    ret = client->readToBuffer(&readErrno);
    if (ret <= 0 && readErrno != EAGAIN)
    {
        closeConn(client);
        return;
    }
    onProcess(client);
}
void Webserver::onProcess(Connect *client)
{
    if (client->handleConn())
    {
        epoller->mod(client->getfd(), connectionEvent | EPOLLOUT);
    }
    else
    {
        epoller->mod(client->getfd(), connectionEvent | EPOLLIN);
    }
}
void Webserver::onWrite(Connect *client)
{
    int ret = -1;
    int writeErrno = 0;
    ret = client->WriteFromBuffer(&writeErrno);
    if (client->writelen() == 0)
    {
        if (client->isKeepAlive())
        {
            onProcess(client);
            return;
        }
    }
    else if (ret < 0)
    {
        if (writeErrno == EAGAIN)
        {
            epoller->mod(client->getfd(), connectionEvent | EPOLLOUT);
            return;
        }
    }
    closeConn(client);
}
void Webserver::Start()
{
    int timeMS = timeout; // epoll wait timeout==-1就是无事件一直阻塞
    if (!isClose)
    {
        std::cout << "Server Start!";
        std::cout << std::endl;
    }
    while (!isClose)
    {
        int eventCnt = epoller->wait(timeMS);
        for (int i = 0; i < eventCnt; ++i)
        {
            int fd = epoller->getFd(i);
            uint32_t events = epoller->getEvents(i);
            if (fd == listenFd)
                handleListen();
            else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                assert(user.count(fd) > 0);
                closeConn(&user[fd]);
            }
            else if (events & EPOLLIN)
            {
                assert(user.count(fd) > 0);
                handleRead(&user[fd]);
            }
            else if (events & EPOLLOUT)
            {
                assert(user.count(fd) > 0);
                handleWrite(&user[fd]);
            }
            else
            {
                std::cout << "Unexpected event" << std::endl;
            }
        }
    }
}