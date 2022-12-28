#include <stdio.h> //printf
#include <stdlib.h> //free
#include <omp.h>
#include <string.h>
#include "array.h"
#include "transpositionSort.h"

#define SEED 4567
#define N 100000
#define FILE_NAME "results.csv"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void PrintUsage(char* argv[])
{
    fprintf(stderr, "Usage: %s [-d][-n maxArraySize]\n", argv[0]);
}

void PrintDone()
{
    printf(ANSI_COLOR_GREEN " DONE" ANSI_COLOR_RESET "\n");
}

int main(int argc, char* argv[])
{
    bool demoMode = false;
    int maxNumberOfElements = N;

#pragma region args handling
    int optind;    
    for (optind = 1; optind < argc ; optind++) 
    {
        if (argv[optind][0] != '-')
            continue;
         
        switch (argv[optind][1])
        {
            case 'd': 
                demoMode = true; 
                break;
            case 'n': 
                if(optind + 1 < argc && argv[optind + 1][0] != '-')
                {
                    maxNumberOfElements = atoi(argv[optind + 1]);
                }
                else
                {
                    PrintUsage(argv);                
                    exit(EXIT_FAILURE);
                }

                if(maxNumberOfElements < 10)
                {
                    fprintf(stderr, "Wrong maxArraySize value, make sure it is greater or equal to 10.");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                PrintUsage(argv);
                exit(EXIT_FAILURE);
        }   
    }
    argv += optind;
#pragma endregion

    //Sprawdzenie liczby procesorów
    int procs_num;
    procs_num = omp_get_num_procs();
    printf("Number of available processors: %d\n", procs_num);

    if(demoMode)
    {
        //Generowanie losowej tablicy o parzystej wielkości
        Array array = GenerateEvenSizeRandomElementArray(SEED, maxNumberOfElements, 0, 100);
        printf("Input array size: %d\n\n", maxNumberOfElements);

        //Sortowanie
        double time;
        printf("Sequential version. Sorting...");
        Array sortedArray = TranspositionSort(array, false, procs_num, &time);
        PrintDone();

        //Wypisywanie czasu
        printf("Sorting time (sequential): %lf s\n\n", time);

        //Sortowanie, wersja z OpenMP
        printf("Parallel version. Sorting...");
        Array sortedArray_OpenMP = TranspositionSort(array, true, procs_num, &time);
        PrintDone();

        //Wypisywanie czasu
        printf("Sorting time (parallel): %lf s\n\n", time);

        //Wyświetlanie małych tablic
        if(maxNumberOfElements <= 30)
        {
            printf("Array:\n");
            PrintElementArray(array);
            printf("Sorted array (sequential):\n");
            PrintElementArray(sortedArray);
            printf("Sorted array (parallel):\n");
            PrintElementArray(sortedArray_OpenMP);
        }
    }//if
    else
    {
        printf("Max array size: %d\n\n", maxNumberOfElements);

        //Przygotowanie pliku do zapisania wyników
        FILE* fp;
        char* filename = FILE_NAME;

        //Tworzenie nowego pliku
        remove(filename);
        fp = fopen(filename, "w+");

        //Tworzenie nagłówków pliku csv
        fprintf(fp,"Id, Parallel, Num_Threads, Available_cores, N, time[s]\n");

        printf("Sequential version.\n");
        int id = 0;
        //Wykonanie sekwencyjne
        bool parallel = false; 
        for (int n = 100; n <= maxNumberOfElements; n *= 10)
        {
            double time;
            Array array = GenerateEvenSizeRandomElementArray(SEED, n, 0, n);

            printf("Input array size: %d. Sorting...", n);

            //Sortowanie
            Array sortedArray = TranspositionSort(array, parallel, 1, &time);

            PrintDone();

            //Zapisywanie wyników w pliku csv
            fprintf(fp, "%d, %s, na, %d, %d, %lf\n",
            id, parallel ? "true" : "false", procs_num, n, time);

            id++;
            //Zwalnianie pamięci
            free(array.elements);
            free(sortedArray.elements);
        }

        
        printf("\nParallel version.\n");
        //Zmiana na równoległe wykonanie
        parallel = true; 

        for (int num_threads = 2; num_threads <= procs_num;  num_threads++)
        {
            for (int n = 100; n <= maxNumberOfElements; n *= 10)
            {        
                double time;
                Array array = GenerateEvenSizeRandomElementArray(SEED, n, 0, n);

                printf("Input array size: %d. Number of threads: %d. Sorting...", n, num_threads);

                //Sortowanie
                Array sortedArray = TranspositionSort(array, parallel, num_threads, &time);

                PrintDone();

                //Zapisywanie wyników w pliku csv
                fprintf(fp, "%d, %s, %d, %d, %d, %lf\n",
                id, parallel ? "true" : "false", num_threads, procs_num, n, time);

                id++;

                //Zwalnianie pamięci
                free(array.elements);
                free(sortedArray.elements);
            }  
        }

        //Zamykanie dostępu do pliku
        fclose(fp);

        printf("\nResults saved to %s", filename);
    }//else

    exit(EXIT_SUCCESS);
}