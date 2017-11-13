// compile: $ mpicc oddEven.c -o oddEven -std=c99
// execute: $ mpiexec -n <number of processors> ./oddEven
//          and then input value of n

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <mpi.h>

void randomize(int *local_list, int local_num);
void local_sort(int *local_list, int local_num);
int compute_partner(int phase, int my_rank);
void odd_even_sort(int *local_list, const int *local_n);

int world_size, id, namelen;
int n; // size of global list
int comm_sz; // alias of world_size

int main(int argc, char *argv[])
{
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	double startwtime, execution_time;
	int *global_list = NULL, *local_list = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Get_processor_name(processor_name, &namelen);
	comm_sz = world_size;

	// read in n and broadcast
	if (id == 0)
	{
		printf("please enter number of keys you wish to sort: ");
		scanf("%d", &n);
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	startwtime = MPI_Wtime();

	// calculate number of element in each process and allocate memory
	int local_n[32], displs[32];
	for (int i = 0; i < world_size - 1; i++)
	{
		local_n[i] = n / comm_sz;
		displs[i] = i * (n / comm_sz);
	}
	local_n[world_size - 1] = n - ((comm_sz - 1) * (n / comm_sz));
	displs[world_size - 1] = (comm_sz - 1) * (n / comm_sz);

	if (id == 0)
		global_list = (int *) calloc(n, sizeof(int));
	local_list = (int *) calloc(local_n[id], sizeof(int));

	// random generate keys and locally sort
	srand(time(NULL) + id);
	randomize(local_list, local_n[id]);
	local_sort(local_list, local_n[id]);

	// print out locally sorted list
	if (id == 0)
	{
		MPI_Gatherv(local_list, local_n[0], MPI_INT,
		            global_list, local_n, displs, MPI_INT,
		            0, MPI_COMM_WORLD);

		printf("\nsorted local list:\n");
		for (int _id = 0; _id < comm_sz; _id++)
		{
			printf("id%d: ", _id);
			for (int i = displs[_id]; i < displs[_id] + local_n[_id]; i++)
				printf("%2d ", global_list[i]);
			printf("\n");
			fflush(stdout);
		}
	}
	else
	{
		MPI_Gatherv(local_list, local_n[id], MPI_INT,
		            NULL, NULL, NULL, MPI_INT,
		            0, MPI_COMM_WORLD);
	}

	// perform odd even sort
	odd_even_sort(local_list, local_n);

	// gather data
	if (id == 0)
	{
		MPI_Gatherv(local_list, local_n[0], MPI_INT,
		            global_list, local_n, displs, MPI_INT,
		            0, MPI_COMM_WORLD);

		printf("\nsorted global list:\n");
		for (int _id = 0; _id < comm_sz; _id++)
		{
			printf("id%d: ", _id);
			for (int i = displs[_id]; i < displs[_id] + local_n[_id]; i++)
				printf("%2d ", global_list[i]);
			printf("\n");
		}
		printf("\n");
		fflush(stdout);
	}
	else
	{
		MPI_Gatherv(local_list, local_n[id], MPI_INT,
		            NULL, local_n, displs, MPI_INT,
		            0, MPI_COMM_WORLD);
	}


	// get execution time and print out
	execution_time = MPI_Wtime() - startwtime;
	MPI_Barrier(MPI_COMM_WORLD);
	printf("process %2d execution time: %f\n", id, execution_time);
	fflush(stdout);

	free(global_list);
	free(local_list);
	MPI_Finalize();

	return 0;
}

void odd_even_sort(int *local_list, const int *local_n)
{
	// allocate local_swap to store local_list and local_buffer
	// to store keys from partner process
	int *local_swap = (int *) calloc(local_n[id], sizeof(int));
	int *local_buffer = (int *) calloc(local_n[comm_sz - 1], sizeof(int));

	for (int phase = 0; phase < comm_sz; phase++)
	{
		int partner = compute_partner(phase, id);

		if (partner != MPI_PROC_NULL)
		{
			// send keys to partner and recv from partner
			MPI_Sendrecv(local_list, local_n[id], MPI_INT, partner, 0,
			             local_buffer, local_n[partner], MPI_INT, partner, 0,
			             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// copy local_list into local_swap
			memcpy(local_swap, local_list, local_n[id] * sizeof(int));

			if (id < partner)   // get smaller key
			{
				int buffer_index = 0, swap_index = 0;

				for (int i = 0; i < local_n[id]; i++)
				{
					if (buffer_index >= local_n[partner])
					{
						local_list[i] = local_swap[swap_index++];
						continue;
					}
					if (swap_index >= local_n[id])
					{
						local_list[i] = local_buffer[buffer_index++];
						continue;
					}

					if (local_swap[swap_index] < local_buffer[buffer_index])
						local_list[i] = local_swap[swap_index++];
					else
						local_list[i] = local_buffer[buffer_index++];
				}
			}
			else     // get larger key
			{
				int buffer_index = local_n[partner] - 1;
				int swap_index = local_n[id] - 1;

				for (int i = local_n[id] - 1; i >= 0; i--)
				{
					if (buffer_index < 0)
					{
						local_list[i] = local_swap[swap_index--];
						continue;
					}
					if (swap_index < 0)
					{
						local_list[i] = local_buffer[buffer_index--];
						continue;
					}

					if (local_swap[swap_index] > local_buffer[buffer_index])
						local_list[i] = local_swap[swap_index--];
					else
						local_list[i] = local_buffer[buffer_index--];
				}
			} // end id < partner
		} // end partner != MPI_PROC_NULL
		MPI_Barrier(MPI_COMM_WORLD);
	}

	free(local_buffer);
	free(local_swap);
}

int compute_partner(int phase, int my_rank)
{
	int partner;

	if (phase % 2 == 0)   // even phase
	{
		if (my_rank % 2 != 0) // odd rank
			partner = my_rank - 1;
		else                  // even rank
			partner = my_rank + 1;
	}
	else                 // odd phase
	{
		if (my_rank % 2 != 0) // odd rank
			partner = my_rank + 1;
		else                  // even rank
			partner = my_rank - 1;
	}
	if (partner == -1 || partner == comm_sz)
		partner = MPI_PROC_NULL;

	return partner;
}


// helper functions
void randomize(int *local_list, int local_num)
{
	int i, maximum_key = n >= INT_MAX ? INT_MAX : n;
	for (i = 0; i < local_num; i++)
		local_list[i] = rand() % maximum_key;
}

int local_sort_cmp(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

void local_sort(int *local_list, int local_num)
{
	qsort(local_list, local_num, sizeof(int), local_sort_cmp);
}
