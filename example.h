
#include <windows.h>
#include "ctpl.h"

using namespace std;




void example() {
    cout << "==============================" << endl;
    cout << "------------------------" << endl;
    cout << "Obliczenia przykladowe" << endl;
    cout << "------------------------" << endl;

    vector<int> vec({ 1,23,435,26,23,54,7,3,3 });

    ctpl::thread_pool* tp = new ctpl::thread_pool(4);

    for (int i = 0;i < vec.size();i++)
    {
        auto lambda = [i, &vec](int id) {
            printf("Starting thread %d!\n", i);
            vec[i] = vec[i] * 2;
            Sleep(1000);
            printf("Finishing thread %d!\n", i);
        };
        tp->push(lambda);

    }
    printf("Stop signal transmitted, waiting for threads.\n");
    tp->stop(true);


    cout <<endl<< "Wyniki:" << endl;
    for (int i = 0;i < vec.size();i++)
    {
        cout << vec[i] << " ";
    }

    cout << endl<<endl;
    cout << "==============================" << endl << endl;

}

