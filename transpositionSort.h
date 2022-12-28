#ifndef SEQ_H
#define SEQ_H
#include "array.h"
#include <stdbool.h>

/*
    Sortowanie przez transpozycję
*/
Array TranspositionSort(Array array, bool parallel, int num_threads, double* time);
/*
    Faza parzysta sortowania
*/
void EvenPhase(Array array, bool parallel, int num_threads);
/*
    Faza nieparzysta sortowania
*/
void OddPhase(Array array, bool parallel, int num_threads);

#endif //SEQ_H
