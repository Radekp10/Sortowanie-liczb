#include <mpi.h>
#include <stdlib.h>
#include "mpi_functions.h"

int GetRandomInt(int lowerBound, int upperBound)
{
    return (rand() % (upperBound - lowerBound)) + lowerBound;
}

int *GenerateEvenSizeRandomElementArray(int seed, int numberOfElements,
                                        int lowerBound, int upperBound)
{

    if (lowerBound >= upperBound)
    {
        printf("Warning! Wrong bounds, empty Array returned.\n");
        int *array;
        array = NULL;
        return array;
    }

    //Ustawiamy seed RNG - funkcja rand() za ka�dym razem zwr�ci ten sam
    //ci�g liczb
    srand(seed);

    //Sprawdzamy czy liczba element�w ustawiona jest na parzyst�
    int makeEven = 0;
    if (numberOfElements % 2 == 1)
    {
        printf("Warning! Generating array of uneven size, added additional element.\n");
        //Ustawiamy zmienn� pomocnicz�, kt�ra pozwoli
        //wygenerowa� dodatkowy element
        makeEven = 1;
    }

    int *array;
    numberOfElements = numberOfElements + makeEven;
    array = (int *)malloc((numberOfElements) * sizeof(int));
    //Losujemy parzyst� liczb� element�w
    for (size_t i = 0; i < numberOfElements; i++)
    {
        array[i] = GetRandomInt(lowerBound, upperBound);
    }

    return array;
}

void Get_data(int *global_n_p, int *local_n_p, int n,
              int my_rank, int p, MPI_Comm comm)
{

    if (my_rank == 0)
    {

        *global_n_p = n;
    }
    // blokowanie
    MPI_Bcast(global_n_p, 1, MPI_INT, 0, comm);

    // ilosc elementow dla 1 procesu
    *local_n_p = *global_n_p / p;
}

void Sort(int local_A[], int local_n, int my_rank, int p, MPI_Comm comm)
{
    int phase;
    int *temp_B, *temp_C;
    int even_partner; 
    int odd_partner;  

    
    temp_B = (int *)malloc(local_n * sizeof(int));
    temp_C = (int *)malloc(local_n * sizeof(int));

    
    if (my_rank % 2 != 0)
    { // odd rank 
        even_partner = my_rank - 1;
        odd_partner = my_rank + 1;
        if (odd_partner == p)
            odd_partner = MPI_PROC_NULL; 
    }
    else
    { // even rank 
        even_partner = my_rank + 1;
        if (even_partner == p)
            even_partner = MPI_PROC_NULL; 
        odd_partner = my_rank - 1;
    }

    /* Sortownie lokalnych list */
    qsort(local_A, local_n, sizeof(int), Compare);

    for (phase = 0; phase < p; phase++)
    {
        
        Odd_even_iter(local_A, temp_B, temp_C, local_n, phase,
                      even_partner, odd_partner, my_rank, p, comm);

        
    }
    
    free(temp_B);
    free(temp_C);
} 

void Odd_even_iter(int local_A[], int temp_B[], int temp_C[],
                   int local_n, int phase, int even_partner, int odd_partner,
                   int my_rank, int p, MPI_Comm comm)
{
    MPI_Status status;

    if (phase % 2 == 0)
    { // even phase 
        if (even_partner >= 0)
        { 
            MPI_Sendrecv(local_A, local_n, MPI_INT, even_partner, 0,
                         temp_B, local_n, MPI_INT, even_partner, 0, comm,
                         &status);

            Merge(local_A, temp_B, temp_C, local_n, my_rank < status.MPI_SOURCE);
            
        }
    }
    else
    { // odd phase 
        if (odd_partner >= 0)
        { 
            MPI_Sendrecv(local_A, local_n, MPI_INT, odd_partner, 0,
                         temp_B, local_n, MPI_INT, odd_partner, 0, comm,
                         &status);

            Merge(local_A, temp_B, temp_C, local_n, my_rank < status.MPI_SOURCE);
            
        }
    }
} 

void Merge(int local_A[], int temp_B[], int temp_C[],
           int local_n, int low)
{
    int ai, bi, ci;

    if (low)
    {
        // wstawia najmniejsze z local_A i temp_B do local_A
        ai = bi = ci = 0;
        while (ci < local_n)
        {
            
            if (local_A[ai] <= temp_B[bi])
            {

                temp_C[ci] = local_A[ai];
                ci++;
                ai++;
            }
            else
            {
                
                
                temp_C[ci] = temp_B[bi];
                ci++;
                bi++;
            }
        }
    }
    else
    {
        // wstawia największe z local_A i temp_B do local_A
        ai = bi = ci = local_n - 1;
        while (ci >= 0)
        {
            
            if (local_A[ai] >= temp_B[bi])
            {
                
                temp_C[ci] = local_A[ai];
                ci--;
                ai--;
            }
            else
            {
                
                temp_C[ci] = temp_B[bi];
                ci--;
                bi--;
            }
        }
    }
    
    memcpy(local_A, temp_C, local_n * sizeof(int));
} 



int Compare(const void *a_p, const void *b_p)
{
    int a = *((int *)a_p);
    int b = *((int *)b_p);

    if (a < b)
        return -1;
    else if (a == b)
        return 0;
    else 
        return 1;
} 
void Print_global_list(int local_A[], int local_n, int my_rank, int p,
                       MPI_Comm comm)
{
    int *A;
    int i, n;
    if (my_rank == 0)
    {
        n = p * local_n;
        A = (int *)malloc(n * sizeof(int));
        // Zbiera dane ze wszystkich procesów z grupy, zwraca A
        MPI_Gather(local_A, local_n, MPI_INT, A, local_n, MPI_INT, 0,
                   comm);
        printf("Sorted list:\n");
        for (i = 0; i < n; i++)
            printf("%d ", A[i]);
        printf("\n\n");
        free(A);
    }
    else
    {
        MPI_Gather(local_A, local_n, MPI_INT, A, local_n, MPI_INT, 0,
                   comm);
    }

} 

void Print_local_lists(int local_A[], int local_n,
                       int my_rank, int p, MPI_Comm comm)
{
    int *A;
    int q;
    MPI_Status status;
    
    if (my_rank == 0)
    {
        
        A = (int *)malloc(local_n * sizeof(int));
        Print_list(local_A, local_n, my_rank);
        for (q = 1; q < p; q++)
        {
            MPI_Recv(A, local_n, MPI_INT, q, 0, comm, &status);
            Print_list(A, local_n, q);
            
        }
        free(A);
    }
    else
    {
        MPI_Send(local_A, local_n, MPI_INT, 0, 0, comm);
    }
} 

void Print_list(int local_A[], int local_n, int rank) {
   int i;
   printf("%d: ", rank);
   for (i = 0; i < local_n; i++)
      printf("%d ", local_A[i]);
   printf("\n");
} 
