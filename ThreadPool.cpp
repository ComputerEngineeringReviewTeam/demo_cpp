#include "ctpl.h"
#include <iostream>
#include <windows.h>
using namespace std;

class Test
{
private:
    ctpl::thread_pool* tp;
public:


    void push(function<void()> func)
    {
        tp->push([func](int id) {
            func();
            });
    }
    void wait()
    {
        tp->stop(true);
    }
    void flush()
    {
        tp = new ctpl::thread_pool(4);
    }

    Test()
    {
        tp = new ctpl::thread_pool(4);
    }
};


int main() {
    vector<int> vec({ 1,23,435,26,23,54,7,3,3 });
    Test te;

    for (int i = 0;i < vec.size();i++)
    {
        auto lambda = [i, &vec]() {
            vec[i] = vec[i] * 2;
        };
        te.push(lambda);

    }
    te.wait();
    te.flush();
    for (int i = 0;i < vec.size();i++)
    {
        cout << vec[i] << " ";
    }



    for (int i = 0;i < vec.size();i++)
    {
        auto lambda = [i, &vec]() {
            vec[i] = vec[i] / 2;
        };
        te.push(lambda);
    }
    te.wait();
    te.flush();
    cout << endl << endl;
    for (int i = 0;i < vec.size();i++)
    {
        cout << vec[i] << " ";
    }
}

