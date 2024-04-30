#pragma once

#include <chrono>

using Time = std::chrono::steady_clock;

//Manipulate this value to regulate Thread Pool size
const int THREAD_LIMIT = 4;

//Manipulate this value to regulate size of matrices being multiplied.
const int n = 10;

Matrix generateMatrix(int rows, int cols)
{
    Matrix m1(rows, cols);
    for (int i = 0; i < m1.getRows(); i++)
    {
        double index = 1;
        for (int j = 0; j < m1.getCols(); j++)
        {
            m1.setElement(index, j, i);
            index += 1.0;
        }
    }
    return m1;
}


void test()
{
    cout << "==============================" << endl;
    cout << "------------------------" << endl;
    cout << "Obliczenia testowe" << endl;
    cout << "------------------------" << endl;

    Parallelizer threads(LOTS_OF_THREADS);
    Parallelizer thread_pool(THREAD_POOL, THREAD_LIMIT);
    Parallelizer no_threads(NO_THREADS);


    printf("\nGenerowanie macierzy: m1 - %dx%d, m2 - %dx%d...\n", n, n, n, n);

    Matrix m1 = generateMatrix(n, n);
    Matrix m2 = generateMatrix(n, n);

    cout << "Macierze wygenerowane!" << endl << endl;

   
    m1.setEngine(&no_threads);
    cout << "Obliczanie m1*m2 bez zrownoleglenia..." << endl;
    auto time = Time::now();
    Matrix product = m1 * m2;
    auto now = Time::now();
    cout <<(now - time).count() << "ns"<<endl << endl;

    m1.setEngine(&thread_pool);
    printf("Obliczanie m1*m2 przy uzyciu Thread Poola na %d watkach...\n", THREAD_LIMIT);
    time = Time::now();
    //TODO: Po wykonaniu wsyzstkih (7) zadan w pliku Perallelizer.h odkomentuj ponizsza linie (polecamy skorzystac z przykladu w pliku example.h):
    //product = m1 * m2;
    now = Time::now();
    cout << (now - time).count() <<"ns"<< endl << endl;
    
    m1.setEngine(&threads);
    printf("Obliczanie m1*m2 metoda prymitywna...\n", THREAD_LIMIT);
    time = Time::now();
    product = m1 * m2;
    now = Time::now();
    cout << (now - time).count() << "ns"<<endl << endl;

    cout << "==============================" << endl;
}