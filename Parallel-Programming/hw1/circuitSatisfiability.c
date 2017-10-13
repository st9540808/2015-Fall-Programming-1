/* circuitSatifiability.c solves the Circuit Satisfiability
 *  Problem using a brute-force sequential solution.
 *
 *   The particular circuit being tested is "wired" into the
 *   logic of function 'checkCircuit'. All combinations of
 *   inputs that satisfy the circuit are printed.
 *
 *   16-bit version by Michael J. Quinn, Sept 2002.
 *   Extended to 32 bits by Joel C. Adams, Sept 2013.
 */

#include <stdio.h>     // printf()
#include <limits.h>    // UINT_MAX
#include "mpi.h"

int checkCircuit (int, long);
int clearExceptLsb(int x) {
    return x & (~x + 1);
}
int clearExceptMsb(int x) {
    return x & x - 1;
}
int recv(int recv_id) { // wrapper function for recv
    int partial_count;
    MPI_Recv(&partial_count, 1, MPI_INT, recv_id, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    return partial_count;
}
void send(int send_id, int partial_count) { // wrapper function for send
    MPI_Send(&partial_count, 1, MPI_INT, send_id, 0, MPI_COMM_WORLD);
}


int main (int argc, char *argv[]) {
    long i;               /* loop variable (64 bits) */
    int id = 0;           /* process id */
    int count = 0;        /* number of solutions */

    int world_size, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME]; 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Get_processor_name(processor_name, &namelen);

    double start_time, total_time;
    start_time = MPI_Wtime();
    for (i = id; i <= UINT_MAX; i += world_size) {
        count += checkCircuit(id, i);
    }
 
    if (id == 0) { // master process
        int level = 1; // depth in tree
        for (; level != world_size; level <<= 1)
            count += recv(level);
    } else { // slaves
        int level = 1, boundary = clearExceptLsb(id);
        for (; level != boundary; level <<= 1) 
            count += recv(id + level);
		send(id - level, count);
    }
    total_time = MPI_Wtime() - start_time;

    if (id == 0) {
        printf("\nA total of %d solutions were found.\n\n", count);
        fflush(stdout);
    }
    printf("Process %d finished in %f secs of all %d process. %s\n",
           id, total_time, world_size, processor_name);
    fflush(stdout);

    MPI_Finalize();
    return 0;
}


/* EXTRACT_BIT is a macro that extracts the ith bit of number n.
 *
 * parameters: n, a number;
 *             i, the position of the bit we want to know.
 *
 * return: 1 if 'i'th bit of 'n' is 1; 0 otherwise
 */

#define EXTRACT_BIT(n,i) ( (n & (1<<i) ) ? 1 : 0)


/* checkCircuit() checks the circuit for a given input.
 * parameters: id, the id of the process checking;
 *             bits, the (long) rep. of the input being checked.
 *
 * output: the binary rep. of bits if the circuit outputs 1
 * return: 1 if the circuit outputs 1; 0 otherwise.
 */

#define SIZE 32

int checkCircuit (int id, long bits) {
    int v[SIZE];        /* Each element is a bit of bits */
    int i;

    for (i = 0; i < SIZE; i++)
        v[i] = EXTRACT_BIT(bits,i);

    if ( ( (v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
            && (!v[3] || !v[4]) && (v[4] || !v[5])
            && (v[5] || !v[6]) && (v[5] || v[6])
            && (v[6] || !v[15]) && (v[7] || !v[8])
            && (!v[7] || !v[13]) && (v[8] || v[9])
            && (v[8] || !v[9]) && (!v[9] || !v[10])
            && (v[9] || v[11]) && (v[10] || v[11])
            && (v[12] || v[13]) && (v[13] || !v[14])
            && (v[14] || v[15]) )
            ||
            ( (v[16] || v[17]) && (!v[17] || !v[19]) && (v[18] || v[19])
              && (!v[19] || !v[20]) && (v[20] || !v[21])
              && (v[21] || !v[22]) && (v[21] || v[22])
              && (v[22] || !v[31]) && (v[23] || !v[24])
              && (!v[23] || !v[29]) && (v[24] || v[25])
              && (v[24] || !v[25]) && (!v[25] || !v[26])
              && (v[25] || v[27]) && (v[26] || v[27])
              && (v[28] || v[29]) && (v[29] || !v[30])
              && (v[30] || v[31]) ) ) {
/*        printf ("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n", id,
                v[31],v[30],v[29],v[28],v[27],v[26],v[25],v[24],v[23],v[22],
                v[21],v[20],v[19],v[18],v[17],v[16],v[15],v[14],v[13],v[12],
                v[11],v[10],v[9],v[8],v[7],v[6],v[5],v[4],v[3],v[2],v[1],v[0]);
        fflush (stdout);*/
        return 1;
    } else {
        return 0;
    }
}
