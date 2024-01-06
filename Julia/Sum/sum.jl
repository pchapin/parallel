# This is a simple example of how to use Julia's multi-threading capabilities to add numbers in
# parallel. This example is a lightly modified version of code taken from the Julia
# documentation: https://docs.julialang.org/en/v1/manual/multi-threading/#The-@threads-Macro

using BenchmarkTools

"""
    sum_serial(a)

Compute the sum of the elements of an array `a` serially.
"""
function sum_serial(a)
    # The obvious serial implementation
    s = 0
    for i in a
        s += i
    end
    return s
end


"""
    sum_multithread(a)

Compute the sum of the elements of an array `a` using multiple threads.
"""
function sum_multithread(a)
    # Split problem into subproblems and map the subproblems to threads (mapping step)
    chunks = Iterators.partition(a, length(a) ÷ Threads.nthreads())
    tasks = map(chunks) do chunk
        Threads.@spawn sum_serial(chunk)
    end

    # Wait for all threads to finish and collect the results (reduction step)
    chunk_sums = fetch.(tasks)
    return sum_serial(chunk_sums)
end

# Testing
a = ones(100_000_000)

println("\nRunning serial sum...")
println("\t", sum_serial(a))
println("Benchmarking serial sum...")
@btime sum_serial(a)

println("\nRunning multi-threaded sum...")
println("\t", sum_multithread(a))
println("Benchmarking multi-threaded sum...")
@btime sum_multithread(a)
