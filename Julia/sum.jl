using BenchmarkTools

function sum_serial(a)
    # The obvious serial implementation
    s = 0
    for i in a
        s += i
    end
    return s
end

a = ones(100_000_000)
println("Running serial sum...")
println("\t", sum_serial(a))
println("Benchmarking serial sum...")
@btime sum_serial(a)

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

println("Running multi-threaded sum...")
println("\t", sum_multithread(a))
println("Benchmarking multi-threaded sum...")
@btime sum_multithread(a)
