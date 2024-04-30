#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <functional>
//TODO: Includuj biblioteke CTPL (C++ Thread Pool Library)


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
    //TODO: Utworz zmienna przechowujaca wskaznik na thread poola.

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
            //TODO: Tutaj czekaj az Thread Pool wykona wszystkie zadania.
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

            //TODO: Tutaj dodaj do kolejki thread poola funktor 'func'. Pamietaj, ze funkcje dodawane do thread poola w CTPL musza brac argument typu int (id watku), a tymczasem 'func' nie bierze zadnych argumentow. Zalecamy uzyc funkcji lambda. 


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
            //TODO: Usun thread poola i utworz nowego (wskaznik powienien byc polem klasy, liczba watkow == zmienna 'thread_limit')
        }
    }


    Parallelizer(TYPES mode) :threads(), flags(), mode(mode)
    {
        if (mode == THREAD_POOL)
        {
            //TODO: Utworz nowego thread poola (liczba watkow == zmienna 'thread_limit')
        }

    }

    Parallelizer(TYPES mode, int n) :Parallelizer(mode)
    {
        this->thread_limit = n;
        
    }

    ~Parallelizer()
    {
        //TODO: Usun thread poola
    }
};
