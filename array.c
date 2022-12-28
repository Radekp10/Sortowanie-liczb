//malloc
#include <stdlib.h>
//rand
#include <time.h>
//printf
#include <stdio.h>
#include "array.h"

int GetRandomInt(int lowerBound, int upperBound)
{
    return (rand() % (upperBound - lowerBound)) + lowerBound;
}

Array GenerateEvenSizeRandomElementArray(int seed, int numberOfElements,
                                         int lowerBound, int upperBound)
{
    
    if(lowerBound >= upperBound)
    {
        printf("Warning! Wrong bounds, empty Array returned.\n");
        Array array;
        array.numberOfElements = 0;
        array.elements = NULL;
        return array;
    }

    //Ustawiamy seed RNG - funkcja rand() za każdym razem zwróci ten sam 
    //ciąg liczb
    srand(seed);

    //Sprawdzamy czy liczba elementów ustawiona jest na parzystą
    int makeEven = 0;    
    if (numberOfElements % 2 == 1)
    {
        printf("Warning! Generating array of uneven size, added additional element.\n");
        //Ustawiamy zmienną pomocniczą, która pozwoli 
        //wygenerować dodatkowy element
        makeEven = 1;
    }    

    Array array;
    array.numberOfElements = numberOfElements + makeEven;
    array.elements = malloc((array.numberOfElements)* sizeof(Element));
    //Losujemy parzystą liczbę elementów
    for (size_t i = 0; i < array.numberOfElements; i++)
    {
        array.elements[i].initialIndex = i;
        array.elements[i].value = GetRandomInt(lowerBound, upperBound);
    }
    
    return array;
}

Array CopyArray(Array array)
{
    Array newArray;
    //Alokacja pamięci
    newArray.elements = malloc((array.numberOfElements)* sizeof(Element));
    newArray.numberOfElements = array.numberOfElements;

    //Przepisywanie wartości elementów
    for (int i = 0; i < array.numberOfElements; i++)
    {
        newArray.elements[i] = array.elements[i];
    }
    
    return newArray;
}

void PrintElementArray(Array array)
{
    //Drukowanie wartości elementów
    for (size_t i = 0; i < array.numberOfElements; i++)
    {
        printf("%d ", array.elements[i].value);
    }
    printf("\n");
}