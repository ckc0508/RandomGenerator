# RandomGenerator
Modern random number generator collection in C++

# How to use
auto rng1 = RandomGenerator::Factory::default_rng(seed); //can .split()
auto rng1_1 = rng1.split()
auto rng2 = RandomGenerator::Factory::fast_rng(seed); //cannot .split() but fastest

auto rng3 = RandomGenerator::Factory::set_rng(sfc64, seed); //choose generator


# Benchmark

PS C:\Users\user\Desktop\EA_random> g++ -O3 -fno-move-loop-invariants -fno-unroll-loops main.cpp -o main
PS C:\Users\user\Desktop\EA_random> ./main.exe
==================== RNG Fixed Count Benchmark (100M) ====================
L64X128MixRandom     | Time:   383.93 ms | Speed:      260461976 num/s | Throughput:    2.084 GB/s
L64X256MixRandom     | Time:   371.72 ms | Speed:      269017376 num/s | Throughput:    2.152 GB/s
L64X1024MixRandom    | Time:   531.41 ms | Speed:      188178194 num/s | Throughput:    1.505 GB/s
L32X64MixRandom      | Time:   457.02 ms | Speed:      218808278 num/s | Throughput:    0.875 GB/s
Xoroshiro128++       | Time:   207.00 ms | Speed:      483090620 num/s | Throughput:    3.865 GB/s
Xoshiro256++         | Time:   177.29 ms | Speed:      564038061 num/s | Throughput:    4.512 GB/s
sfc64                | Time:   138.61 ms | Speed:      721469489 num/s | Throughput:    5.772 GB/s
sfc32                | Time:   173.77 ms | Speed:      575484586 num/s | Throughput:    2.302 GB/s
SplitMix32           | Time:   209.62 ms | Speed:      477042792 num/s | Throughput:    1.908 GB/s
SplitMix64           | Time:   243.80 ms | Speed:      410173450 num/s | Throughput:    3.281 GB/s
PCG64DXSM            | Time:   361.87 ms | Speed:      276342409 num/s | Throughput:    2.211 GB/s
PCG64DXSphi          | Time:   363.92 ms | Speed:      274786875 num/s | Throughput:    2.198 GB/s
===========================================================================

PS C:\Users\user\Desktop\EA_random> g++ -O3 -march=native main.cpp -o main
PS C:\Users\user\Desktop\EA_random> ./main.exe
==================== RNG Fixed Count Benchmark (100M) ====================
L64X128MixRandom     | Time:   432.17 ms | Speed:      231388336 num/s | Throughput:    1.851 GB/s
L64X256MixRandom     | Time:   350.74 ms | Speed:      285110909 num/s | Throughput:    2.281 GB/s
L64X1024MixRandom    | Time:   465.21 ms | Speed:      214957841 num/s | Throughput:    1.720 GB/s
L32X64MixRandom      | Time:   377.78 ms | Speed:      264707758 num/s | Throughput:    1.059 GB/s
Xoroshiro128++       | Time:   215.86 ms | Speed:      463264943 num/s | Throughput:    3.706 GB/s
Xoshiro256++         | Time:   204.26 ms | Speed:      489583139 num/s | Throughput:    3.917 GB/s
sfc64                | Time:   134.60 ms | Speed:      742941498 num/s | Throughput:    5.944 GB/s
sfc32                | Time:   155.91 ms | Speed:      641402259 num/s | Throughput:    2.566 GB/s
SplitMix32           | Time:   200.72 ms | Speed:      498194294 num/s | Throughput:    1.993 GB/s
SplitMix64           | Time:   190.33 ms | Speed:      525407939 num/s | Throughput:    4.203 GB/s
PCG64DXSM            | Time:   362.30 ms | Speed:      276010695 num/s | Throughput:    2.208 GB/s
PCG64DXSphi          | Time:   361.13 ms | Speed:      276906905 num/s | Throughput:    2.215 GB/s
===========================================================================

PS C:\Users\user\Desktop\EA_random> g++ -O3 main.cpp -o main
PS C:\Users\user\Desktop\EA_random> ./main.exe
==================== RNG Fixed Count Benchmark (100M) ====================
L64X128MixRandom     | Time:   381.10 ms | Speed:      262397219 num/s | Throughput:    2.099 GB/s
L64X256MixRandom     | Time:   325.20 ms | Speed:      307504587 num/s | Throughput:    2.460 GB/s
L64X1024MixRandom    | Time:   468.60 ms | Speed:      213400073 num/s | Throughput:    1.707 GB/s
L32X64MixRandom      | Time:   386.53 ms | Speed:      258710123 num/s | Throughput:    1.035 GB/s
Xoroshiro128++       | Time:   219.49 ms | Speed:      455608887 num/s | Throughput:    3.645 GB/s
Xoshiro256++         | Time:   177.65 ms | Speed:      562920114 num/s | Throughput:    4.503 GB/s
sfc64                | Time:   132.62 ms | Speed:      754047159 num/s | Throughput:    6.032 GB/s
sfc32                | Time:   168.84 ms | Speed:      592268643 num/s | Throughput:    2.369 GB/s
SplitMix32           | Time:   198.11 ms | Speed:      504776956 num/s | Throughput:    2.019 GB/s
SplitMix64           | Time:   195.63 ms | Speed:      511159637 num/s | Throughput:    4.089 GB/s
PCG64DXSM            | Time:   364.47 ms | Speed:      274373639 num/s | Throughput:    2.195 GB/s
PCG64DXSphi          | Time:   383.71 ms | Speed:      260615997 num/s | Throughput:    2.085 GB/s
===========================================================================

