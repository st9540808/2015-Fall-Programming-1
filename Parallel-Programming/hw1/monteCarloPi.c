#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"


int clearExceptLsb(int x) {
    return x & (~x + 1);
}
int my_recv(int recv_id) { // wrapper function for recv
    int partial_count;
    MPI_Recv(&partial_count, 1, MPI_INT, recv_id, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    return partial_count;
}
void my_send(int send_id, int partial_count) { // wrapper function for send
    MPI_Send(&partial_count, 1, MPI_INT, send_id, 0, MPI_COMM_WORLD);
}
double fRand() {
    return -1.0f + 2.0f * (double) rand() / RAND_MAX;
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "require argv[1]=number of tosses\n");
        exit(EXIT_FAILURE);
    }
    
    int world_size, id, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME]; 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Get_processor_name(processor_name, &namelen);

    srand(time(NULL) + id);
    long number_of_tosses = strtol(argv[1], NULL, 10);
    long toss = id, number_in_circle = 0;

    double start_time, total_time;
    start_time = MPI_Wtime();
    for (; toss < number_of_tosses; toss += world_size) {
        double x = fRand(), y = fRand();
        double distance_squared = x*x + y*y;
        if (distance_squared < 1.0f)
            number_in_circle++;
    }
    total_time = MPI_Wtime() - start_time;

    if (id == 0) { // master process
        int level = 1; // depth in tree
        for (; level != world_size; level <<= 1)
            number_in_circle += my_recv(level);
    } else { // slaves
        int level = 1, boundary = clearExceptLsb(id);
        for (; level != boundary; level <<= 1) 
            number_in_circle += my_recv(id + level);
        my_send(id - level, number_in_circle);
    }

    printf("Process %d finished calulating in %f secs of all %d process. %s\n",
           id, total_time, world_size, processor_name);
    fflush(stdout);

    MPI_Barrier(MPI_COMM_WORLD);
    if (id == 0) {
        printf("%.10f\n", 4 * number_in_circle / (double) number_of_tosses);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}
