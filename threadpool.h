#pragma once

#include "locker.h"
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
  private:
    int m_threadNumber;
    bool stop;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    locker mtx, resource, resempty;

  public:
    explicit ThreadPool(int threadNumber, int queMaxSize);
    ~ThreadPool();
    void Submit(std::function<void()>);
};