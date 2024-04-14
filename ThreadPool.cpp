#include "CTPL/ctpl_stl.h"
#include <iostream>
#include <windows.h>
using namespace std;
void first(int id,int i) {
    std::cout <<i<< "hello from " << id << '\n';
    Sleep(100);
}

int main() {


    ctpl::thread_pool p(4);

    for(int i=0;i<100;i++)
        p.push(first,i);  // function

    
    p.stop(true);
}

