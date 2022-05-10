#include "buffer.h"

size_t Buffer::remain()
{
    return buffer.size() - tail + head;
}
Buffer::Buffer(int initSize) : buffer(initSize), head(0), tail(0)
{
}
void Buffer::init()
{
    bzero(&buffer[0], buffer.size());
    head = tail = 0;
}
const char *Buffer::gethead() const
{
    return (&*buffer.begin()) + head;
}
const char *Buffer::gettail() const
{
    return (&*buffer.begin()) + tail;
}
size_t Buffer::getLen()
{
    return tail - head;
}
void Buffer::updhead(size_t len)
{
    head += len;
}
void Buffer::updtail(size_t len)
{
    tail += len;
}
void Buffer::allocate(size_t len)
{
    if (remain() < len)
        buffer.resize(tail + len + 1);
    else
    {
        std::copy((&*buffer.begin()) + head, (&*buffer.begin()) + tail, &*buffer.begin());
        tail = tail - head;
        head = 0;
    }
}
void Buffer::ensure(size_t len)
{
    if (buffer.size() - tail < len)
        allocate(len);
}
void Buffer::append(const char *str, size_t len)
{
    ensure(len);
    std::copy(str, str + len, (&*buffer.begin()) + tail);
    updtail(len);
}
void Buffer::append(const std::string &str)
{
    append(str.data(), str.length());
}
std::string Buffer::tostr()
{
    std::string rnt(buffer.begin() + head, buffer.begin() + tail);
    return rnt;
}
ssize_t Buffer::readFd(int fd, int *error)
{
    char buf[65536];
    ssize_t len = recv(fd, buf, 65535, 0);
    if (len <= 0)
        *error = errno;
    else
        append(buf, len);
    return len;
}
ssize_t Buffer::writeFd(int fd, int *error)
{
    ssize_t len = send(fd, gethead(), getLen(), 0);
    // ssize_t len = write(fd, gethead(), getLen());
    if (len < 0)
        *error = errno;
    else
        head += len;
    return len;
}