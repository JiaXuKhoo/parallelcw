Complete the table below with your results, and then fill in the final two sections at the end.

Please do not edit or remove table or section headings, as the autograder script uses these to
locate the start and end of the table.

Each row of the table will be split into columns using the Python "split()" method. Therefore,
- fill in each column with values;
- only use whitespace between columns;
- do not add any non-numeric characters (which would cause the value to be read as zero);
- do not worry if your columns are not perfectly aligned.

For the parallel speed-up S, please note that:
- the time you should use is already output by the provided code;
- take as the serial execution time the time output by the code when run with a single process.
  Hence, the speed-up for 1 process in the table below must be 1.00.


No. Machines:   Total No. Processes:     Mean time (average of 3 runs) in seconds:        Parallel speed-up, S:
=============   ====================     =========================================        =====================
1                       1                                  0.0901                                 1                 
1                       2                                  0.0581                                 1.55            
1                       4                                  0.0450                                 2.01                                 
1                       8                                  0.0403                                 2.24                
2                       16                                 1.19                                   0.09                                              
2                       32                                 1.20                                   0.0752                                     

Please state the number of cores per machine (for Bragg 2.05, this is typically 12): 12

A brief interpretation of your results:

Observation from the result indicates that there is good scaling on the single machine from 1 to 2.24 in parallel speed-up. However, there was a decreasing percentage
increase in terms of parallel speed-up. As the number of machines increased to 2, the time taken to run increased and there is a less
than 1 parallel speed up. I interpreted this as the overhead of communication and network latency dominating over computation.
Another interpretation is that as the machine has 12 physical cores, the use of oversubscribing causes more overhead in terms of
the context switching required to upkeep concurrency.


