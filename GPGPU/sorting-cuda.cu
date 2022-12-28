#include <stdio.h> //printf
#include <time.h>
#include <stdlib.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define SEED 4567
#define N 10000
#define THREADS_PER_BLOCK 512


//Prototypy funkcji
__global__ void EvenPhase(int* array, int numberOfElements);
__global__ void OddPhase(int* array, int numberOfElements);
int * TranspositionSort(int* array, int numberOfElements, double* time);
int GetRandomInt(int lowerBound, int upperBound);
int * GenerateEvenSizeRandomElementArray(int seed, int* numberOfElements,
    int lowerBound, int upperBound);
int * CopyArray(int* array, int numberOfElements);
void PrintElementArray(int* array, int numberOfElements);


__global__ void EvenPhase(int* array, int numberOfElements)
{
    //Globalny ID watku
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    //Zmienne pomocnicza
    int tmp = 0;
    int upperBound = (int)numberOfElements / 2;
    //Sprawdzenie czy nie wychodzimy poza granice
    if (id < upperBound)
    {
        //Porównujemy pary elementów (x0,x1), (x2, x3), ...
        //Jeœli x[i] > x[i+1] to zamieniamy miejscami
        if (array[2 * id] > array[2 * id + 1])
        {
            tmp = array[2 * id];
            array[2 * id] = array[2 * id + 1];
            array[2 * id + 1] = tmp;
        }
    }
}


__global__ void OddPhase(int* array, int numberOfElements)
{
    //Globalny ID watku
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    //Zmienne pomocnicza
    int tmp = 0;
    int upperBound = ((int)numberOfElements / 2) - 1;
    //Sprawdzenie czy nie wychodzimy poza granice
    if (id < upperBound)
    {
        //Porównujemy pary elementów (x1,x2), (x3, x4), ...
        //Jeœli x[i] > x[i+1] to zamieniamy miejscami
        if (array[2 * id + 1] > array[2 * id + 2])
        {
            tmp = array[2 * id + 1];
            array[2 * id + 1] = array[2 * id + 2];
            array[2 * id + 2] = tmp;
        }
    }
}


int * TranspositionSort(int* array, int numberOfElements, double* time)
{
    //Czasy rozpoczecia i zakonczenia sortowania
    clock_t start, stop;
    //Sprawdzanie warunków pocz¹tkowych sortowania
    if (numberOfElements % 2 == 1)
    {
        printf("Uneven number of elements! Array hasn't been sorted.");
        return array;
    }

    //Kopie zmiennych na hoscie
    int* newArray = CopyArray(array, numberOfElements);

    //Kopie zmiennych na device
    int* d_newArray;

    //Wielkosc pamieci do zaalokowania
    int array_size = numberOfElements * sizeof(int);

    //Alokacja pamieci na device dla kopii zmiennych
    cudaMalloc(&d_newArray, array_size);

    //Rozmiar gridu
    int gridSizeEvenPhase, gridSizeOddPhase;

    // Liczba blokow w gridzie (dzielimy przez 2, bo elemnety badane sa parami)
    gridSizeEvenPhase = (int)numberOfElements / 2;
    gridSizeOddPhase = (int)numberOfElements / 2 - 1; //Zerowy i ostatni element tablicy nie maja pary, wiec liczba iteracji jest o 1 mniejsza

    //Liczba blokow
    //Zabezpieczenie, by nie bylo za malo blokow (gdy numberOfElements nie dzieli sie na NUM_THREADS)
    int numBlocksEvenPhase = (gridSizeEvenPhase + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;
    int numBlocksOddPhase = (gridSizeOddPhase + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;

    //Fazy sortowania
    start = clock();
    cudaMemcpy(d_newArray, newArray, array_size, cudaMemcpyHostToDevice);
    for (int n = 1; n < numberOfElements + 1; n++)
    {
        if (n % 2 == 0)
        {
            //Faza parzysta
            EvenPhase << <numBlocksEvenPhase, THREADS_PER_BLOCK >> > (d_newArray, numberOfElements);
        }
        else
        {
            //Faza nieparzysta 
            OddPhase << <numBlocksOddPhase, THREADS_PER_BLOCK >> > (d_newArray, numberOfElements);
        }
    }
    cudaMemcpy(newArray, d_newArray, array_size, cudaMemcpyDeviceToHost);
    stop = clock();

    if (time != NULL)
        *time = ((double)(stop - start)) / CLOCKS_PER_SEC;

    //Czyszczenie pamieci na device
    cudaFree(d_newArray);

    return newArray;
}


int GetRandomInt(int lowerBound, int upperBound)
{
    return (rand() % (upperBound - lowerBound)) + lowerBound;
}


int * GenerateEvenSizeRandomElementArray(int seed, int* numberOfElements,
    int lowerBound, int upperBound)
{

    if (lowerBound >= upperBound)
    {
        printf("Warning! Wrong bounds, empty Array returned.\n");
        int * array;
        array = NULL;
        return array;
    }

    //Ustawiamy seed RNG - funkcja rand() za ka¿dym razem zwróci ten sam 
    //ci¹g liczb
    srand(seed);

    //Sprawdzamy czy liczba elementów ustawiona jest na parzyst¹
    int makeEven = 0;
    if (*numberOfElements % 2 == 1)
    {
        printf("Warning! Generating array of uneven size, added additional element.\n");
        //Ustawiamy zmienn¹ pomocnicz¹, która pozwoli 
        //wygenerowaæ dodatkowy element
        makeEven = 1;
    }

    int * array;
    *numberOfElements = *numberOfElements + makeEven;
    array = (int*)malloc((*numberOfElements) * sizeof(int));
    //Losujemy parzyst¹ liczbê elementów
    for (size_t i = 0; i < *numberOfElements; i++)
    {
        array[i] = GetRandomInt(lowerBound, upperBound);
    }

    return array;
}


int * CopyArray(int * array, int numberOfElements)
{
    int * newArray;
    //Alokacja pamiêci
    newArray = (int*)malloc((numberOfElements) * sizeof(int));

    //Przepisywanie wartoœci elementów
    for (int i = 0; i < numberOfElements; i++)
    {
        newArray[i] = array[i];
    }

    return newArray;
}


void PrintElementArray(int * array, int numberOfElements)
{
    //Drukowanie wartoœci elementów
    for (size_t i = 0; i < numberOfElements; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}



int main(int argc, char* argv[])
{
    int maxNumberOfElements = N;

	//Generowanie losowej tablicy o parzystej wielkoœci
	int* array;
	array = GenerateEvenSizeRandomElementArray(SEED, &maxNumberOfElements, 0, 100);
	printf("Input array size: %d\n\n", maxNumberOfElements);

	//Sortowanie
	double time;
	printf("CUDA version. Sorting...\n");
	int* sortedArray = TranspositionSort(array, maxNumberOfElements, &time);

	//Wypisywanie czasu
	printf("Sorting time (CUDA): %lf s\n\n", time);

	//Wyœwietlanie ma³ych tablic
	if (maxNumberOfElements <= 30)
	{
		printf("Array:\n");
		PrintElementArray(array, maxNumberOfElements);
		printf("Sorted array (CUDA):\n");
		PrintElementArray(sortedArray, maxNumberOfElements);
	}
    //Zwolnienie pamieci
	free(array);
	free(sortedArray);

    exit(EXIT_SUCCESS);
}