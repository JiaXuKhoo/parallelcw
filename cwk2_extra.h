//
// This file contains functions used in the second item of coursework. Do not alter these routines
// (or copy-and-paste their content to cwk2.c and change it there), as a different version of
// this file will be used as part of the assessment.
//

// Get the problem size N from the command line and allocates memory for A, x and b, and initialise
// the values for A and x. Should only be called by rank 0. Returns -1 in case of error.
int initialiseRank0( int argc, char **argv, int numprocs, int rank, int *N, float **A, float **x, float **b )
{
	int row, col;

	// Get and check the command line argument, which should be the problem size N.
	if( argc!=2 )
	{
		if( rank==0 ) printf( "Error: Must enter the matrix/vector size N as the only command line argument.\n" );
		return -1;
	}

	int argVal = atoi( argv[1] );		// Note argv[0] is the executable name! 
	if( argVal<=0 )						// atoi() converts ASCII to an integer value; returns 0 for non-numeric strings.
	{
		if( rank==0 ) printf( "Could not convert the command line argument '%s' to a positive integer.\n", argv[1] );
		return -1;
	}
	if( argVal%numprocs )
	{
		if( rank==0 ) printf( "The problem size N=%i is not a multiple of the number of processes=%i.\n", argVal, numprocs );
		return -1;
	}

	// Only initialise N and the arrays on rank 0 only.
	if( rank==0 )
	{
		// Set the problem size.
		*N = argVal;

		// Allocate memory for the matrix and vectors.
		*A = (float*) malloc( (*N)*(*N)*sizeof(float) );
		*x = (float*) malloc( (*N)     *sizeof(float) );
		*b = (float*) malloc( (*N)     *sizeof(float) );

		if( !(*A) || !(*x) || !(*b) )
		{
			printf( "Cannot allocate memory for the matrix and the two vectors.\n");
			return -1;
		}

		// Initialise A and x with random values.
		srand( time(0) );							// Set the seed for the random number generator.

		for( row=0; row<(*N); row++ )
			for( col=0; col<(*N); col++ )
				(*A)[row*(*N)+col] = 1.0f * rand() / RAND_MAX;
		
		for( row=0; row<(*N); row++ )
			(*x)[row] = 1.0f * rand() / RAND_MAX;
	}

	return 0;
}


// Checks against the serial calculation and outputs a success or failure message.
void checkAgainstSerial( float *A, float *x, float *b, int N )
{
	int row, col, success=1;

	// Message explaining the full check will not be displayed.
	printf( "Checking against serial calculation [only showing at most 10 rows]\n" );

	// Check the calculation row-by-row.
	for( row=0; row<N; row++ )
	{
		// Get the answer in serial.
		float answer = 0.0;
		for( col=0; col<N; col++ )
			answer += A[row*N+col] * x[col];

		// Display up to 10 rows.
		if( row<10 )
			printf( "Row %i:\tParallel answer=%g\tSerial answer=%g\n",row,b[row],answer );

		// Check the serial answer matches the parallel answer, allowing for finite precision errors.
		if( b[row]!=b[row] || fabs(answer-b[row]) > 1e-5 )		// 'b[row]!=b[row]' evaluates as 'true' if b[row]==nan.
		{
			printf( "Error: Matrix-vector multiplication failed for at least one row.\n" );
			success = 0;
			break;
		}
	}		

	// Display a success message if all rows appeared to be correct.
	if( success ) printf( "Matrix-vector multiplication successful.\n" );
}
