#!/usr/bin/python3

import numpy as np
import timeit

# Read the coefficients.
input_file = open("../TestData/2000x2000.dat")

size = 0
i = 0
j = 0
for line in input_file:
    # If we haven't seen the size yet, deal with that right away (it's in the first line).
    if size == 0:
        size = int(line)

        # Create the arrays of the appropriate size up front.
        a = np.zeros((size, size), dtype=float)
        b = np.zeros((size), dtype=float)

    else:
        if j < size:
            # Get row i.
            a[i, j] = float(line)
            j += 1
        else:
            # Get the driving vector value. Reset for next row.
            b[i] = float(line)
            i += 1
            j  = 0
input_file.close()
print("Coefficients read")

# The elimination step...
print("Elimination...")

start_time = timeit.default_timer()
result = np.linalg.solve(a, b)
stop_time = timeit.default_timer()

# Print result...
print("\nSolution is")
print(result)
    
print("Time =", stop_time - start_time, "seconds")
