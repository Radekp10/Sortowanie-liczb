#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int GetRandomInt(int lowerBound, int upperBound);

int *GenerateEvenSizeRandomElementArray(int seed, int numberOfElements,
                                        int lowerBound, int upperBound);

void Get_data(int *global_n_p, int *local_n_p, int n,
              int my_rank, int p, MPI_Comm comm);

void Sort(int local_A[], int local_n, int my_rank,
          int p, MPI_Comm comm);

void Odd_even_iter(int local_A[], int temp_B[], int temp_C[],
                   int local_n, int phase, int even_partner, int odd_partner,
                   int my_rank, int p, MPI_Comm comm);

void Print_global_list(int local_A[], int local_n, int my_rank, int p,
                       MPI_Comm comm);

int Compare(const void *a_p, const void *b_p);

void Print_local_lists(int local_A[], int local_n,
                       int my_rank, int p, MPI_Comm comm);

void Print_list(int local_A[], int local_n, int rank);

void Merge(int local_A[], int temp_B[], int temp_C[],
           int local_n, int low);