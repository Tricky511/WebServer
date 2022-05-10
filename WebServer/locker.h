#pragma once

#include <exception>
#include <iostream>
#include <semaphore.h>
#include <stdexcept>
class locker
{
  private:
    sem_t sem;

  public:
    explicit locker(int pshared, unsigned value)
    {
        sem_init(&sem, pshared, value);
    }
    ~locker()
    {
        sem_destroy(&sem);
    }
    bool wait()
    {
        int ret = sem_wait(&sem);
        return ret == 0;
    }
    bool signal()
    {
        int ret = sem_post(&sem);
        return ret == 0;
    }
};
