#include <stdio.h> //printf
#include <time.h>
#include <stdlib.h>

#define SEED 4567
#define N 10000

//Prototypy funkcji
void EvenPhase(int* array, int numberOfElements);
void OddPhase(int* array, int numberOfElements);
int* TranspositionSort(int* array, int numberOfElements, double* time);
int GetRandomInt(int lowerBound, int upperBound);
int* GenerateEvenSizeRandomElementArray(int seed, int* numberOfElements,
    int lowerBound, int upperBound);
int* CopyArray(int* array, int numberOfElements);
void PrintElementArray(int* array, int numberOfElements);


void EvenPhase(int* array, int numberOfElements)
{
    //Por�wnujemy pary element�w (x0,x1), (x2, x3), ...
    //Je�li x[i] > x[i+1] to zamieniamy miejscami
    int tmp = 0;
    int upperBound = (int)numberOfElements / 2;
    for (int i = 0; i <upperBound; i ++)
    {
        if (array[2 * i] > array[2 * i + 1])
        {
            tmp = array[2 * i];
            array[2 * i] = array[2 * i + 1];
            array[2 * i + 1] = tmp;
        }
    }
}


void OddPhase(int* array, int numberOfElements)
{
    //Por�wnujemy pary element�w (x1,x2), (x3, x4), ...
    //Je�li x[i] > x[i+1] to zamieniamy miejscami
    int tmp = 0;
    int upperBound = ((int)numberOfElements / 2) - 1;
    for (int i = 0; i < upperBound; i ++)
    {
        if (array[2 * i + 1] > array[2 * i + 2])
        {
            tmp = array[2 * i + 1];
            array[2 * i + 1] = array[2 * i + 2];
            array[2 * i + 2] = tmp;
        }
    }
}


int* TranspositionSort(int* array, int numberOfElements, double* time)
{
    //Czasy rozpoczecia i zakonczenia sortowania
    clock_t start, stop;
    //Sprawdzanie warunk�w pocz�tkowych sortowania
    if (numberOfElements % 2 == 1)
    {
        printf("Uneven number of elements! Array hasn't been sorted.");
        return array;
    }

    //Tworzenie g��bokiej kopii tablicy
    int * newArray = CopyArray(array, numberOfElements);

    //Fazy sortowania
    start = clock();
    for (int n = 1; n < numberOfElements + 1; n++)
    {
        if (n % 2 == 0)
        {
            //Faza parzysta
            EvenPhase(newArray, numberOfElements);
        }
        else
        {
            //Faza nieparzysta 
            OddPhase(newArray, numberOfElements);
        }
    }
    stop = clock();

    if (time != NULL)
        *time = ((double)(stop - start)) / CLOCKS_PER_SEC;

    return newArray;
}


int GetRandomInt(int lowerBound, int upperBound)
{
    return (rand() % (upperBound - lowerBound)) + lowerBound;
}


int* GenerateEvenSizeRandomElementArray(int seed, int* numberOfElements,
    int lowerBound, int upperBound)
{

    if (lowerBound >= upperBound)
    {
        printf("Warning! Wrong bounds, empty Array returned.\n");
        int* array;
        array = NULL;
        return array;
    }

    //Ustawiamy seed RNG - funkcja rand() za ka�dym razem zwr�ci ten sam 
    //ci�g liczb
    srand(seed);

    //Sprawdzamy czy liczba element�w ustawiona jest na parzyst�
    int makeEven = 0;
    if (*numberOfElements % 2 == 1)
    {
        printf("Warning! Generating array of uneven size, added additional element.\n");
        //Ustawiamy zmienn� pomocnicz�, kt�ra pozwoli 
        //wygenerowa� dodatkowy element
        makeEven = 1;
    }

    int* array;
    *numberOfElements = *numberOfElements + makeEven;
    array = (int*)malloc((*numberOfElements) * sizeof(int));
    //Losujemy parzyst� liczb� element�w
    for (size_t i = 0; i < *numberOfElements; i++)
    {
        array[i] = GetRandomInt(lowerBound, upperBound);
    }

    return array;
}


int* CopyArray(int* array, int numberOfElements)
{
    int* newArray;
    //Alokacja pami�ci
    newArray = (int*)malloc((numberOfElements) * sizeof(int));

    //Przepisywanie warto�ci element�w
    for (int i = 0; i < numberOfElements; i++)
    {
        newArray[i] = array[i];
    }

    return newArray;
}


void PrintElementArray(int* array, int numberOfElements)
{
    //Drukowanie warto�ci element�w
    for (size_t i = 0; i < numberOfElements; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}



int main(int argc, char* argv[])
{
    int maxNumberOfElements = N;

	//Generowanie losowej tablicy o parzystej wielko�ci
	int* array;
	array = GenerateEvenSizeRandomElementArray(SEED, &maxNumberOfElements, 0, 100);
	printf("Input array size: %d\n\n", maxNumberOfElements);

	//Sortowanie
	double time;
	printf("Sequential version. Sorting...\n");
	int* sortedArray = TranspositionSort(array, maxNumberOfElements, &time);

	//Wypisywanie czasu
	printf("Sorting time (sequential): %lf s\n\n", time);

	//Wy�wietlanie ma�ych tablic
	if (maxNumberOfElements <= 30)
	{
		printf("Array:\n");
		PrintElementArray(array, maxNumberOfElements);
		printf("Sorted array (sequential):\n");
		PrintElementArray(sortedArray, maxNumberOfElements);
	}

	//Zwolnienie pamieci
	free(array);
	free(sortedArray);

    exit(EXIT_SUCCESS);
}