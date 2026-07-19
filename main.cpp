#include "RandomGenerator.hpp"
#include <iostream>
#include <chrono>
#include <tuple>
#include <iomanip>

// Helper to get element size at compile time
template <typename T>
constexpr double get_bytes_size() {
    return sizeof(typename T::result_type);
}

template <typename RNGType>
void run_fixed_count_test(RNGType& rng, uint64_t TARGET_COUNT = 100000000) {    
    std::cout << "Testing [" << RNGType::id << "] ... " << std::flush;

    // 仅在开头读一次时间
    auto start_time = std::chrono::high_resolution_clock::now();

    // 纯粹干净的死循环，没有任何打断和杂音
    volatile typename RNGType::result_type dummy_accumulator = 0;
    for (uint64_t i = 0; i < TARGET_COUNT; ++i) {
        dummy_accumulator ^= rng(); 
    }

    // 仅在结尾读一次时间
    auto end_time = std::chrono::high_resolution_clock::now();
    
    // 计算耗时（秒）
    double duration_seconds = std::chrono::duration<double>(end_time - start_time).count();

    // 公式换算
    double speed_per_second = TARGET_COUNT / duration_seconds;
    double bytes_per_element = get_bytes_size<RNGType>();
    double gbps = (speed_per_second * bytes_per_element) / 1e9; // 1 GB = 10^9 bytes

    // 换算毫秒用于辅助直观观察
    double duration_ms = duration_seconds * 1000.0;

    std::cout << "\r" << std::left << std::setw(20) << RNGType::id 
              << " | Time: " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << duration_ms << " ms"
              << " | Speed: " << std::right << std::setw(14) << static_cast<uint64_t>(speed_per_second) << " num/s"
              << " | Throughput: " << std::fixed << std::setprecision(3) << std::setw(8) << gbps << " GB/s\n";
}

int main() {
    constexpr uint64_t DEFAULT_SEED = 42;
    constexpr uint64_t N = 100000000;

    auto rng_tuple = std::make_tuple(
        RandomGenerator::Factory::set_rng(l64x128m, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(l64x256m, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(l64x1024m, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(l32x64m, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(xoroshiro128plusplus, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(xoshiro256plusplus, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(sfc64, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(sfc32, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(pcg64dxsm, DEFAULT_SEED),
        RandomGenerator::Factory::set_rng(pcg64dxsPhi, DEFAULT_SEED)
    );

    std::cout << "==================== RNG Fixed Count Benchmark (" << (N / 1000000) <<"M) ====================\n";

    std::apply([](auto&... rngs) {
        (run_fixed_count_test(rngs, N), ...);
    }, rng_tuple);

    std::cout << "===========================================================================\n";
    return 0;
}


// g++ -O3 -fno-move-loop-invariants -fno-unroll-loops main.cpp -o main
// g++ -O3 -fdisable-tree-cunrolli -march=native main.cpp -o main
// g++ -O3 -march=native main.cpp -o main
// g++ -O3 main.cpp -o main
// ./main.exe
