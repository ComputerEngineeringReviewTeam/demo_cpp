
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define LOTS_OF_THREADS 0 

#define LIMITED_THREADS 1

#define THREAD_POOL 2

using namespace std;

class Matrix
{
private: 
    vector<double>elements;
    int rows;
    int cols;

public:

    void setElement(double element, int col, int row)
    {
        elements[row * cols + col] = element;
    }

    double getElement(int col, int row)
    {
        return elements[row * cols + col];
    }



    Matrix(int cols, int rows) :elements()
    {
        elements.resize(cols * rows);
        this->rows = rows;
        this->cols = cols;
    }

};
//TODO: W tym miejscu trzeba nadpisac operator mnozenia i zaimplementowac w nim mnozenie macierzy. Do tego w tej klasie trzeba dac pole typu Parallelizer* i tego obiektu bedzie uzywal operator mnozenia

//TODO: Przeniesc wszystkie klasy do oddzielnych plikow (osobno headery i body, czyli .h i .cpp), bo klasy beda uzywac siebie nawzajem.

class Parallelizer
{
private:
    int mode = LOTS_OF_THREADS;
    int thread_limit = 3;
    vector<thread>threads;
    vector<bool>flags;
    vector<mutex*>mutexes;
    condition_variable cv;


public:

    template<typename F>
    void parallelize(const F* func)
    {
        if (mode == LOTS_OF_THREADS)
        {
            flags.push_back(false);
            mutex* nm = new mutex;
            mutexes.push_back(nm);
            int thread_num = threads.size();
            threads.emplace_back([func, thread_num, this](){
                 
                
                (*func)();

                {
                    
                    lock_guard<mutex> lk(*mutexes[thread_num]);
                    flags[thread_num] = true;
                }
                cv.notify_all();
            });
        }

        //TODO: Implementacja Thread Poola - Liczba watkow to pole thread_limit. Threadpool musi byc tworzony w konstruktorze klasy i resetowany (czyli usuwany i tworzony od nowa) w metodzie flush(). 
        //W tym miejscu powinno byc tylko dodawanie zadan dla threadpoola.
        //W metodzie wait_until_done() trzeba zaimplementowac oczekiwanie az ten Thread Pool skonczy (tylko nie busy wait), thread pool ma pewnie wbudowana metode do tego, trzeba przejrzec dokumentacje.

        //TODO: Implementacja mechanizmu w ktorym liczba dzialajacych watkow jest ograniczona do thread_limit. Trzeba dodac obsluge tego rowniez do metody flush(). Ogolnie najlepiej uzywac istniejacego wektora watkow, flag mutexow itp. 
        //Tylko trzeba zaimplementowac jakies kolejkowanie.
    }

    void wait_until_done()
    {
        if (mode == LOTS_OF_THREADS || mode == LIMITED_THREADS)
        {
            int index = 0;
            while (true)
            {
                if (index == flags.size())
                {
                    break;
                }
                unique_lock<mutex> lk(*mutexes[index]);
                if (flags[index] == false)
                {


                    cv.wait(lk, [this, index] {return flags[index]; });

                }
                if (flags[index] == true)
                {
                    threads[index].join();
                    index++;
                }
                lk.unlock();
            }
        }
       

    }

    //Metoda resetuje ca³y obiekt
    void flush()
    {
        threads.clear();
        for (int i = 0; i < mutexes.size(); i++)
        {
            delete mutexes[i];
        }
        mutexes.clear();
        flags.clear();
    }

    Parallelizer(int mode) :threads(),flags(),mutexes()
    {
        this->mode = mode;
    }

};



int main()
{
    vector<int>a = { 1, 2, 3, 4 };
    vector<int>b = { 5, 6, 7, 8 };
    vector<int>results;
    vector<int>results2;
    Parallelizer par(LOTS_OF_THREADS);

    auto lambda1 = [a, b, &results]() {
        for (int i = 0; i < a.size(); i++)
        {
            results.push_back(a[i] + b[i]);
        }
    };

    auto lambda2 = [a, b, &results2]() {
        for (int i = 0; i < a.size(); i++)
        {
            results2.push_back(a[i] - b[i]);
        }
    };
    par.parallelize(&lambda1);
    par.parallelize(&lambda2);
    par.wait_until_done();
    int r = 1 + 2;
    for (int i = 0; i < results.size(); i++)
    {
        cout << results[i] <<endl;
    }
    for (int i = 0; i < results2.size(); i++)
    {
        cout << results2[i] << endl;
    }
    return 1;
}
