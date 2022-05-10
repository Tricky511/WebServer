#include "threadpool.h"

ThreadPool::ThreadPool(int threadNumber, int queMaxSize)
    : m_threadNumber(threadNumber), mtx(0, 1), resource(0, 0), resempty(0, queMaxSize)
{
    for (int i = 0; i < threadNumber; i++)
    {
        threads.emplace_back([this]() {
            while (!stop)
            {
                resource.wait();
                mtx.wait();

                auto task = tasks.front();
                tasks.pop();

                resempty.signal();
                mtx.signal();

                task();
            }
        });
    }
}
ThreadPool::~ThreadPool()
{
    stop = true;
    for (int i = 0; i < m_threadNumber; ++i)
        if (threads[i].joinable())
            threads[i].join();
}
void ThreadPool::Submit(std::function<void()> task)
{
    resempty.wait();
    mtx.wait();

    tasks.push(task);

    resource.signal();
    mtx.signal();
}