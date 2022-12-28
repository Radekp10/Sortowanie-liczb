#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "mpi_functions.h"

#define n 20

int main(int argc, char *argv[])
{
   int my_rank, p;
   int *local_A;
   int global_n;
   int local_n;
   MPI_Comm comm;
   double start, finish, loc_elapsed, elapsed;

   MPI_Init(&argc, &argv);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &p);
   MPI_Comm_rank(comm, &my_rank);

   Get_data(&global_n, &local_n, n, my_rank, p, comm);

   local_A = GenerateEvenSizeRandomElementArray(my_rank, n, 0, n);


   Print_local_lists(local_A, local_n, my_rank, p, comm);

   MPI_Barrier(comm);

   start = MPI_Wtime();

   Sort(local_A, local_n, my_rank, p, comm);

   finish = MPI_Wtime();
   loc_elapsed = finish - start;
   MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

   Print_global_list(local_A, local_n, my_rank, p, comm);

   free(local_A); 

   if (my_rank == 0)
      printf("Sorting time %f milliseconds \n", loc_elapsed * 1000);

   MPI_Finalize();

   return 0;
} 
