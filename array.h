#ifndef ARRAY_H
#define ARRAY_H

/*
    Element sortowanej tablicy.
    value: wartość po której sortujemy
    initialIndex: indeks do śledzenia elementu, np. czy sortowanie jest stabline 
*/
typedef struct Element
{
    int initialIndex;
    int value;
}Element;

/*
    Pomocnicza struktura do zarządzania tablicą.
    elements: elementy tablicy
    numberOfElements: rozmiar tablicy
*/
typedef struct Array
{
    Element* elements;
    int numberOfElements;
}Array;

/*
    Generowanie losowego integera z zakresu [LOWER_BOUND, UPPER_BOUND)
*/
int GetRandomInt(int lowerBound, int upperBound);
/*
    Generowanie tablicy o parzystej liczbie losowych elementów
*/
Array GenerateEvenSizeRandomElementArray(int seed, int numberOfElements,
                                         int lowerBound, int upperBound);
/*
    Kopiowanie tablicy struktury Array
*/
Array CopyArray(Array array);
/*
    Wyświetlanie struktury Array
*/
void PrintElementArray(Array array);

#endif //ARRAY_H