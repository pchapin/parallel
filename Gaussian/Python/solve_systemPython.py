#!/usr/bin/python3

import timeit

# Read the coefficients.
input_file = open("../TestData/2000x2000.dat")

size = 0
i = 0
j = 0
a = [ [] ]
b = []
for line in input_file:
    # If we haven't seen the size yet, deal with that right away (it's in the first line).
    if size == 0:
        size = int(line)
    else:
        if j < size:
            # Get row i.
            a[i].append(float(line))
            j += 1
        else:
            # Get the driving vector value. Reset for next row.
            b.append(float(line))
            i += 1
            j  = 0
            a.append([])
input_file.close()
print("Coefficients read")

# The elimination step...
print("Elimination...")
start_time = timeit.default_timer()
for i in range(0, size):
    print(i)
    
    # Find the row with the largest |a[j, i]|, j = i, ..., n - 1
    k = i;
    m = abs(a[i][i])
    for j in range(i + 1, size):
        if abs(a[j][i]) > m:
            k = j
            m = abs(a[j][i])

    # Check for |a[k, i]| zero.
    if abs(a[k][i]) <= 1.0E-6:
           print("System is degenerate!")
           exit(1)

    # Exchange row i and row k if necessary.
    if k != i:
           temp = a[i].copy()
           a[i] = a[k].copy()
           a[k] = temp

           temp = b[i]
           b[i] = b[k]
           b[k] = temp

    # Subtract multiples of row i from subsequent rows.
    for j in range(i + 1, size):
           m = a[j][i] / a[i][i]  # Scale factor

           for k in range(0, size):
               a[j][k] -= m*a[i][k]
           b[j] -= m*b[i]


# Back substitution step...
for i in range(size - 1, -1, -1):
    if abs(a[i][i]) <= 1.0E-6:
        print("System is degenerate!")
        exit(1)

    sum = b[i]
    for j in range(i + 1, size):
        sum -= a[i][j] * b[j]
    b[i] = sum / a[i][i]

# Print result...
print("\nSolution is")
for i in range(0, size):
    print(" x(%4d) = %9.5f" % (i, b[i]))
    
print("Time =", timeit.default_timer() - start_time, "seconds")
