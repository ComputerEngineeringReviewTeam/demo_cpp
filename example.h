
#include <windows.h>
#include "ctpl.h"

using namespace std;




void example() {
    cout << "==============================" << endl;
    cout << "------------------------" << endl;
    cout << "Obliczenia przykladowe" << endl;
    cout << "------------------------" << endl;

    vector<int> vec({ 1,23,435,26,23,54,7,3,3 });

    //Tworzymy Thread Poola na 4 watkach
    ctpl::thread_pool tp(4);

    for (int i = 0;i < vec.size();i++)
    {
        //Tworzymy funkcje lambda - musi ona brac argument typu int, jest nim ID watku
        auto lambda = [i, &vec](int id) {
            printf("Starting task %d on thread %d...\n", i, id);
            vec[i] = vec[i] * 2;
            Sleep(1000);
            printf("Finishing task %d!\n", i);
        };

        //Dodajemy lambde do kolejki thread poola
        tp.push(lambda);

    }
    printf("Stop signal transmitted, waiting for threads.\n");

    //Czekamy az thread pool zakonczy prace wywolujac funkcje 'stop' z argumentem true (true oznacza, ze czekamy az watki skoncza),
    //TODO: Sprawdz co sie stanie, gdy wywolamy funkcje 'stop' z domyslna wartoscia argumentu (false)
    tp.stop(true);


    cout <<endl<< "Wyniki:" << endl;
    for (int i = 0;i < vec.size();i++)
    {
        cout << vec[i] << " ";
    }

    cout << endl<<endl;
    cout << "==============================" << endl << endl;

}

