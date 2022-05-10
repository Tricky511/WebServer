#include "timer.h"

void TimeHeap::swap(size_t x, size_t y)
{
    std::swap(heap[x], heap[y]);
    lis[heap[x].fd] = x;
    lis[heap[y].fd] = y;
}
void TimeHeap::down(int pos)
{
    int son = 0;
    int n = heap.size();
    for (; pos * 2 + 1 < n; pos = son)
    {
        son = pos * 2 + 1;
        if (son + 1 < n && heap[son + 1].expire < heap[son].expire)
            ++son;
        if (heap[son].expire < heap[pos].expire)
            swap(pos, son);
        else
            break;
    }
}
void TimeHeap::add(int fd, int timeout, std::function<void()> cb)
{
    if (lis.count(fd))
    {
        upd(fd, timeout);
        return;
    }
    lis[fd] = heap.size();
    heap.push_back({fd, Clock ::now() + Ms(timeout), cb});
    int pos = lis[fd], fa = 0;
    for (; pos > 0; pos = fa)
    {
        fa = (pos - 1) / 2;
        if (heap[fa].expire <= heap[pos].expire)
            break;
        swap(pos, fa);
    }
}
void TimeHeap::upd(int fd, int timeout)
{

    if (!lis.count(fd))
        return;
    TimeNode &timer = heap[lis[fd]];
    timer.expire = Clock::now() + Ms(timeout);
    down(lis[fd]);
}
void TimeHeap::pop()
{
    if (!heap.size())
        return;
    lis.erase(heap[0].fd);
    heap[0] = heap.back();
    lis[heap[0].fd] = 0;
    heap.pop_back();
    down(0);
}
void TimeHeap::tick()
{

    if (!heap.size())
        return;
    while (heap.size())
    {
        TimeNode node = heap.front();
        if (std::chrono::duration_cast<Ms>(node.expire - Clock::now()).count() > 0)
            break;
        node.cb();
        pop();
    }
}
int TimeHeap::nextTime()
{
    tick();
    size_t res = -1;
    if (heap.size())
    {
        res = std::chrono::duration_cast<Ms>(heap[0].expire - Clock::now()).count();
        if (res < 0)
            res = 0;
    }
    return res;
}