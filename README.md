# RandomGenerator
Modern random number generator collection in C++

# How to use
auto rng1 = RandomGenerator::Factory::default_rng(seed); //can .split()

auto rng1_1 = rng1.split()

auto rng2 = RandomGenerator::Factory::fast_rng(seed); //cannot .split() but fastest

auto rng3 = RandomGenerator::Factory::set_rng(sfc64, seed); //choose generator

std::uniform_int_distribution dist(1, 100);

for (int i = 0; i < 20; ++i){

    std::cout << dist(rng2) << "\n";
    
}

//can use with C++ distribution

if u dont pass parameter seed to the generator, std::random_device will auto generate a random number as a seed
