#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include "ctpl.h"


enum TYPES {
    LOTS_OF_THREADS,
    NO_THREADS,
    THREAD_POOL
};

class Parallelizer
{
private:
    const TYPES mode = LOTS_OF_THREADS;
    int thread_limit = 3;
    vector<thread>threads;
    vector<bool>flags;
    ctpl::thread_pool* tp;

public:


  

    void wait_until_done()
    {
        if (mode == LOTS_OF_THREADS)
        {
            int index = 0;
            while (true)
            {
                if (index == flags.size())
                {
                    break;
                }

                if (flags[index] == false)
                {
                    continue;  
                }
                else
                {
                    threads[index].join();
                    index++;
                }               
            }
        }
        else if(mode == THREAD_POOL)
        {
            tp->stop(true);
        }

    }



    void parallelize(function<void()> func)
    {
        if (mode == LOTS_OF_THREADS)
        {
            flags.push_back(false);
            int thread_num = threads.size();

            threads.emplace_back([func, thread_num, this]() {
                func();
                this->flags[thread_num] = true;
            });
        }
        else if (mode == THREAD_POOL)
        {

            tp->push([func](int id) {
                func();
            });


        }
        else if (mode == NO_THREADS)
        {
            func();
        }
    }

    void flush()
    {
        if (mode == LOTS_OF_THREADS)
        {
            threads.clear();
            flags.clear();
        }
        else
        {
            delete tp;
            tp = new ctpl::thread_pool(this->thread_limit);
        }
    }


    Parallelizer(TYPES mode) :threads(), flags(), mode(mode)
    {
        if (mode == THREAD_POOL)
        {
            tp = new ctpl::thread_pool(this->thread_limit);
        }

    }

    Parallelizer(TYPES mode, int n) :threads(), flags(), mode(mode)
    {
        this->thread_limit = n;
        if (mode == THREAD_POOL)
        {
            tp = new ctpl::thread_pool(this->thread_limit);
        }
    }

    ~Parallelizer()
    {
        delete tp;
    }
};
