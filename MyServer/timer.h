#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds Ms;
class TimeNode
{
  public:
    int fd;
    Clock::time_point expire;
    std::function<void()> cb;
};
class TimeHeap
{
  private:
    std::vector<TimeNode> heap;
    std::unordered_map<int, int> lis;
    void down(int pos);
    void swap(size_t x, size_t y);

  public:
    TimeHeap()
    {
        heap.reserve(64);
    }
    void add(int fd, int timeout, std::function<void()> cb);
    void upd(int fd, int timeout);
    void pop();
    void tick();
    int nextTime();
};