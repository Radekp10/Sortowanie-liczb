#include <stdio.h> //printf
#include <omp.h>
#include "transpositionSort.h"

Array TranspositionSort(Array array, bool parallel, int num_threads, double* time)
{
    //Czasy rozpoczecia i zakonczenia sortowania
    double start, stop;
    //Sprawdzanie warunków początkowych sortowania
    if(array.numberOfElements % 2 == 1)
    {
        printf("Uneven number of elements! Array hasn't been sorted.");
        return array;
    }
    //Tworzenie głębokiej kopii tablicy
    Array newArray = CopyArray(array);

    //Fazy sortowania
    start = omp_get_wtime();
    for (int n = 1; n < array.numberOfElements + 1; n++)
    {
        if (n % 2 == 0)
        {
            //Faza parzysta
            EvenPhase(newArray, parallel, num_threads);
        }
        else
        {
            //Faza nieparzysta
            OddPhase(newArray, parallel, num_threads);
        }

    }
    stop = omp_get_wtime();

    if(time != NULL)
        *time = stop-start;

    return newArray;
}

void EvenPhase(Array array, bool parallel, int num_threads)
{
    //int threads_num, thread_id;  //Można odkomentować tu i poniżej, żeby wypisał, jakie będą id wątków (najlepiej dla małych danych)

    #pragma omp parallel if(parallel),  num_threads(num_threads)
    {
        //thread_id = omp_get_thread_num();
        //threads_num = omp_get_num_threads();
        //printf("Thread ID: %d out of %d \n" , thread_id, threads_num);
        #pragma omp for schedule(static)
            //Porównujemy pary elementów (x0,x1), (x2, x3), ...
            for (int i = 0; i < array.numberOfElements; i += 2)
            {
                //Jeśli x[i] > x[i+1] to zamieniamy miejscami
                if(array.elements[i].value > array.elements[i+1].value)
                {
                    Element tmp;
                    tmp = array.elements[i];
                    array.elements[i] = array.elements[i+1];
                    array.elements[i+1] = tmp;
                }
            }
    }

}

void OddPhase(Array array, bool parallel, int num_threads)
{
    #pragma omp parallel if(parallel), num_threads(num_threads)
    {
        #pragma omp for schedule(static)
            //Porównujemy pary elementów (x1,x2), (x3, x4), ...
            for (int i = 1; i < array.numberOfElements - 1; i += 2)
            {
                //Jeśli x[i] > x[i+1] to zamieniamy miejscami
                if(array.elements[i].value > array.elements[i+1].value)
                {
                    Element tmp;
                    tmp = array.elements[i];
                    array.elements[i] = array.elements[i+1];
                    array.elements[i+1] = tmp;
                }
            }
    }

}
