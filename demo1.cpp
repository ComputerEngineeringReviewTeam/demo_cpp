
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>

#include "ctpl.h"


enum TYPES {
    LOTS_OF_THREADS,
    LIMITED_THREADS,
    THREAD_POOL
};

using namespace std;


class Parallelizer
{
private:
    TYPES mode = LOTS_OF_THREADS;
    int thread_limit = 3;
    vector<thread>threads;
    vector<bool>flags;
    vector<mutex*>mutexes;
    condition_variable cv;
    ctpl::thread_pool *tp;

public:

    void parallelize(function<void()> func)
    {
        if (mode == LOTS_OF_THREADS)
        {
            flags.push_back(false);
            mutex* nm = new mutex;
            mutexes.push_back(nm);
            int thread_num = threads.size();

            threads.emplace_back([func, thread_num, this]() {


                func();


                {
                    lock_guard<mutex> lk(*mutexes[thread_num]);

                    flags[thread_num] = true;
                }
                cv.notify_all();

                });
        }
        else if(mode == THREAD_POOL)
        {

            tp->push([func](int id) {
                func();
                
            });  // lambda


        }

        //TODO: Implementacja Thread Poola - Liczba watkow to pole thread_limit. Threadpool musi byc tworzony w konstruktorze klasy i resetowany (czyli usuwany i tworzony od nowa) w metodzie flush(). 
        //W tym miejscu powinno byc tylko dodawanie zadan dla threadpoola.
        //W metodzie wait_until_done() trzeba zaimplementowac oczekiwanie az ten Thread Pool skonczy (tylko nie busy wait), thread pool ma pewnie wbudowana metode do tego, trzeba przejrzec dokumentacje.

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

                if (flags[index] == false)
                {

                    unique_lock<mutex> lk(*mutexes[index]);
                    cv.wait(lk, [this, index] {return flags[index]; });

                }

                threads[index].join();
                index++;

            }
              
        }
        else
        {
            tp->stop(true);
        }

    }

    //Metoda zmieniajaca wielkość poola
    void resizeTP(int n)
    {
        tp->stop(false);
        this->thread_limit = n;
        tp->resize(this->thread_limit);
    }

    //Metoda resetuje caly obiekt
    void flush()
    {   
        if (this->mode != THREAD_POOL)
        {
            threads.clear();
            for (int i = 0; i < mutexes.size(); i++)
            {
                delete mutexes[i];
            }
            mutexes.clear();
            flags.clear();
        }
        else
        {
            tp= new ctpl::thread_pool(this->thread_limit);
        }
        
    }

    Parallelizer(TYPES mode) :threads(), flags(), mutexes()
    {
        this->mode = mode;
        tp = new ctpl::thread_pool(this->thread_limit);
    }

    Parallelizer(TYPES mode, int n) :threads(), flags(), mutexes()
    {
        this->mode = mode;
        this->thread_limit = n;
        tp = new ctpl::thread_pool(this->thread_limit);
    }
};


class Matrix
{
private:
    vector<double>elements;
    int rows;
    int cols;
    Parallelizer* parallel_engine;



public:

    void setElement(double element, int col, int row)
    {
        elements[row * cols + col] = element;
    }

    double getElement(int col, int row) const
    {
        return elements[row * cols + col];
    }

    int getRows() const
    {
        return rows;
    }

    int getCols() const
    {
        return cols;
    }

    void setEngine(Parallelizer* engine)
    {
        parallel_engine = engine;
    }


    Matrix(int rows, int cols) :elements()
    {
        elements.resize(cols * rows);
        this->rows = rows;
        this->cols = cols;
    }

    Matrix(Matrix& old)
    {
        cols = old.getCols();
        rows = old.getRows();
        elements.resize(cols * rows);
        for (int i = 0; i < elements.size(); i++)
        {
            elements[i] = old.elements[i];
        }
    }

    Matrix(Matrix&& old)
    {
        cols = old.getCols();
        rows = old.getRows();
        elements.resize(cols * rows);
        for (int i = 0; i < elements.size(); i++)
        {
            elements[i] = old.elements[i];
        }
    }

    Matrix& operator=(const Matrix& right)
    {
        cols = right.getCols();
        rows = right.getRows();
        elements.resize(cols * rows);
        for (int i = 0; i < elements.size(); i++)
        {
            elements[i] = right.elements[i];
        }
        return *this;
    }

    friend Matrix operator*(Matrix& left, Matrix& right);

};

Matrix operator*(Matrix& left, Matrix& right)
{

    left.parallel_engine->flush();
    int new_rows = left.getRows();
    int new_cols = right.getCols();
    int subvector_number = right.getRows();
    double* first_step_vectors = (double*)malloc(sizeof(double) * (new_rows * subvector_number * new_cols));
    int offset = 0;

    for (int i = 0; i < new_cols; i++)
    {

        for (int j = 0; j < subvector_number; j++)
        {
            double scalar = right.getElement(i, j);
            left.parallel_engine->parallelize([=, &left]() {
                
                for (int k = 0; k < new_rows; k++)
                {
                    first_step_vectors[offset + k] = scalar * left.getElement(j, k);
                }
                
                });
           
            offset += new_rows;
        }
    }

    left.parallel_engine->wait_until_done();
    left.parallel_engine->flush();

    Matrix result(new_cols, new_rows);

    for (int i = 0; i < new_cols; i++)
    {

        auto lambda = [i, new_rows, subvector_number, first_step_vectors, &result]() {
            for (int j = 0; j < new_rows; j++)
            {
                double sum = 0;
                for (int k = 0; k < subvector_number; k++)
                {
                    sum += first_step_vectors[i * subvector_number * new_rows + new_rows * k + j];
                }
                result.setElement(sum, i, j);
            }
        };
        left.parallel_engine->parallelize(lambda);
    }
    
    left.parallel_engine->wait_until_done();
    left.parallel_engine->flush();
    free(first_step_vectors);
    return result;
}


int main()
{
    Parallelizer par(THREAD_POOL);

    

    Matrix m1(3, 2);
    double index = 1;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            m1.setElement(index, j, i);
            index += 1.0;
        }
    }

    Matrix m2(2, 3);
    index = 1;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            m2.setElement(index, j, i);
            index += 1.0;
        }

    }
    m1.setEngine(&par);

    Matrix product = m1 * m2;

    for (int i = 0; i < product.getRows(); i++)
    {
        for (int j = 0; j < product.getCols(); j++)
        {
            cout << product.getElement(j, i) << " ";
        }
        cout << endl;
    }

    return 1;
}