//
// COMP/XJCO3221 Parallel Computation Coursework 2: MPI / Distributed Memory Parallelism.
//


//
// Includes.
//

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// The MPI library.
#include <mpi.h>

// The include file for this coursework. Do not change the routines in "cwk2_extra.h", and this will be changed
// with a different file for the purposes of assessment.
#include "cwk2_extra.h"


int checkPowerofTwo(int numprocs){
	return (numprocs & (numprocs - 1)) == 0;
}

void distributeN(int *N, int rank, int numprocs){
	// If not power of two
	if (!checkPowerofTwo(numprocs)){
		MPI_Bcast(N, 1, MPI_INT, 0, MPI_COMM_WORLD);
		return;
	}

	int k = 0;
	while( (1 << k) < numprocs )
	{
		int sendernum = 1 << k;

		// All senders send to corresponding recievers
		if( rank < sendernum )
		{
			int recievernum = rank + sendernum;
			if( recievernum < numprocs )
				MPI_Send( N, 1, MPI_INT, recievernum, 0, MPI_COMM_WORLD );
		}
		// All recievers recieve
		else if( rank < 2 * sendernum )
		{
			int src = rank - sendernum;
			MPI_Recv( N, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
		}

		k++;
	}
}

//
//
//
int main( int argc, char *argv[] )
{
	//
	// Initialisation.
	//

	// Start MPI and get the rank of this process.
	int numprocs, rank;
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	// At the start of the calculation, only rank 0 knows the matrix/vector size N and the values of both
	// the vector and the matrix. For this assessment, N is extracted from command line arguments and the
	// vector and matrix are randomised by the routine initialiseRank0() in cwk2_extras.h
	int N=0;
	float *A=NULL, *x=NULL, *b=NULL;
	if( initialiseRank0(argc,argv,numprocs,rank,&N,&A,&x,&b)==-1 )	// Allocates memory for A, x and b; initialises A and x.
	{
		MPI_Finalize();												// initialiseRank0() returns -1 if there was a problem.
		return EXIT_FAILURE;
	}

	//
	// Perform matrix-vector multiplication in parallel.
	//

	// Start the timing.
	double startTime = MPI_Wtime();

	// Step 1. Communicate N from rank 0 to all processes using MPI functions.
	distributeN(&N, rank, numprocs);

	// Step 2. All process allocate local memory based on N, where N is divisible by numprocs.
	// Assuming that N is always divisible by numprocs
	int localRows = N / numprocs;

	// Allocate memory for Rows
	float *localA = (float*) malloc(localRows * N * sizeof(float));
	// Allocate memory for output vector elements
	float *localB = (float*) malloc(localRows * sizeof(float));
	// Allocate memory for input vector
	if( rank != 0 ){
		x = (float*) malloc(N * sizeof(float));
	}

	// Check if memory allocation was successful
	if( !localA || !localB || !x ){
	printf("Memory allocation failed on rank %d\n", rank);
	MPI_Finalize();
	return EXIT_FAILURE;
	}

	// Step 3. Distribute the full vector x from rank 0 to all processes.
	MPI_Bcast(x, N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// Step 4. Distribute rows of the matrix A to all processes.
	MPI_Scatter(A, localRows*N, MPI_FLOAT, localA, localRows*N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// Step 5. Perform the calculations on each process.
	int row, col;
	for(row=0; row<localRows; row++)
	{
		localB[row] = 0.0f; // init output to 0
		for(col=0; col<N; col++)
			localB[row] += localA[row*N + col] * x[col]; // Calculate result
	}

	// Step 6. Communicate the full solution vector back to rank 0.
	MPI_Gather(localB, localRows, MPI_FLOAT, b, localRows, MPI_FLOAT, 0, MPI_COMM_WORLD);

	//
	// Check the answer on rank 0 in serial. Also output the result of the timing.
	//
	if( rank==0 )
	{
		// Time for the parallel computation.
		printf( "Parallel calculation took a total time %g s.\n", MPI_Wtime() - startTime );

		// Call the check routine "checkAgainstSerial()" in cwk2_extra.h, which prints messages explaining if the parallel calculation
		// matches the serial one, to within floating-point precision.
		checkAgainstSerial(A,x,b,N);					// Do NOT modify this call or the function in cwk2_extra.h 
	}

	//
	// Finalise.
	//

	// Free up all dynamically-allocated memory.
	if( rank==0 )
	{
		free( x );
		free( A );
		free( b );
	}

	// Tell MPI to free up resources, then quit.
	MPI_Finalize();
	return EXIT_SUCCESS;
}

