#ifndef RANDOMGENERATOR_HPP
#define RANDOMGENERATOR_HPP

#include <cstdlib>
#include <random>
#include <string>
#include <type_traits>

struct sfc64_t {};
struct sfc32_t {};
struct l64x128m_t {};
struct l64x256m_t {};
struct l64x1024m_t {};
struct l32x64m_t {};
struct xoroshiro128_t {};
struct xoshiro256_t {};
struct splitmix32_t {};
struct splitmix64_t {};
struct pcg64dxsm_t {};
struct pcg64dxsphi_t {};

inline constexpr sfc64_t sfc64{};
inline constexpr sfc32_t sfc32{};
inline constexpr l64x128m_t l64x128m{};
inline constexpr l64x256m_t l64x256m{};
inline constexpr l64x1024m_t l64x1024m{};
inline constexpr l32x64m_t l32x64m{};
inline constexpr xoroshiro128_t xoroshiro128plusplus{};
inline constexpr xoshiro256_t xoshiro256plusplus{};
inline constexpr splitmix32_t splitmix32{};
inline constexpr splitmix64_t splitmix64{};
inline constexpr pcg64dxsm_t pcg64dxsm{};
inline constexpr pcg64dxsphi_t pcg64dxsPhi{};

//----------------------------------------------------------------------------------------------------------------------

class RandomGenerator {
private:

    static constexpr uint64_t GOLDEN_RATIO_64 = 0x9e3779b97f4a7c15ULL;
    static constexpr uint32_t GOLDEN_RATIO_32 = 0x9e3779b9;
    static constexpr uint32_t  SILVER_RATIO_32 = 0x6A09E667;
    static constexpr uint64_t SILVER_RATIO_64 = 0x6A09E667F3BCC909L;

    static constexpr uint64_t L64XM_M = 0xd1342543de82ef95ULL;
    static constexpr uint32_t L32XM_M = 0xadb4a92dULL;

    static inline std::random_device rd{};

    static inline uint64_t mixStafford13(uint64_t z){
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
	    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
	    return z ^ (z >> 31);
    }

    static inline uint32_t mixStafford4(uint64_t z){
        z = (z ^ (z >> 33)) * 0x62a9d9ed799705f5L;
	    return (((z ^ (z >> 28)) * 0xcb24d0a5c88c35b3L) >> 32);
    }

    //murmur 3
    static inline uint64_t mixMurmur64(uint64_t z){
        z = (z ^ (z >> 33)) * 0xff51afd7ed558ccdULL; // MurmurHash3 mix constants
        z = (z ^ (z >> 33)) * 0xc4ceb9fe1a85ec53ULL;
        return (z ^ (z >> 33));
    }

    static inline uint32_t mixMurmur32(uint32_t z){
        z = (z ^ (z >> 16)) * 0x85ebca6b;
        z = (z ^ (z >> 13)) * 0xc2b2ae35;
        return z ^ (z >> 16);
    }

    //murmurhash 3
    static inline uint64_t mixGamma(uint64_t z){
        z = (z ^ (z >> 33)) * 0xff51afd7ed558ccdULL; // MurmurHash3 mix constants
        z = (z ^ (z >> 33)) * 0xc4ceb9fe1a85ec53ULL;
        z = (z ^ (z >> 33));                  
        int n = 64 - __builtin_clzll(z ^ (z >> 1));       // ensure enough transitions
        return (n < 24) ? z ^ 0xaaaaaaaaaaaaaaaaULL : z;
    }

    static inline uint32_t mixGamma32(uint32_t z){
        z = (z ^ (z >> 16)) * 0x85ebca6b;
        z = (z ^ (z >> 13)) * 0xc2b2ae35;
        z = (z ^ (z >> 16));   
        z = (z << 1) | 1;            
        int n = 32 - __builtin_clz(z ^ (z >> 1));       // ensure enough transitions
        return (n < 12) ? z ^ 0xaaaaaaaa : z;
    }

    static inline uint64_t mixLea64(uint64_t z){
        z = (z ^ (z >> 32)) * 0xdaba0b6eb09322e3L;
        z = (z ^ (z >> 32)) * 0xdaba0b6eb09322e3L;
        return (z ^ (z >> 32));
    }

    static inline uint32_t mixLea32(uint32_t z) {
        z = (z ^ (z >> 16)) * 0xd36d884b;
        z = (z ^ (z >> 16)) * 0xd36d884b;
        return z ^ (z >> 16);
    }


    static inline uint64_t _rotl64(uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }
    static inline uint32_t _rotl(uint32_t x, int k) {
        return (x << k) | (x >> (32 - k));
    }

public:

class Factory;

//----------------------------------------------------------------------------------------------------------------------

class SplitMix64 {
    uint64_t seed;
    friend class Factory;

    explicit SplitMix64 (uint64_t seed) : seed(seed){}

    inline uint64_t next(){
        seed += GOLDEN_RATIO_64;
        return mixStafford13(seed);
    }

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "SplitMix64";

    inline result_type operator()(){
        return next();
    }
};

class SplitMix32 {
    uint32_t seed;
    friend class Factory;

    explicit SplitMix32 (uint32_t seed) : seed(seed){}

    inline uint32_t next(){
        seed += GOLDEN_RATIO_32;
        return mixMurmur32(seed);
    }

public:
    using result_type = uint32_t;
    static constexpr uint32_t min() { return 0; }
    static constexpr uint32_t max() { return UINT32_MAX; }
    static constexpr const char* id = "SplitMix32";

    inline result_type operator()(){
        return next();
    }
};

//----------------------------------------------------------------------------------------------------------------------

class Sfc64 {
    uint64_t a, b, c, counter;

    inline uint64_t next(){
        uint64_t tmp = a + b + counter++;
        a = b ^ (b >> 11);
        b = c + (c << 3);
        c = ((c << 24) | (c >> 40)) + tmp;
        return tmp;
    }

    explicit Sfc64(uint64_t seed){
        a = b = c = seed;
        counter = 1;

        for (int i = 0; i < 12; ++i){
            next();
        }
    }

    friend class Factory;

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "sfc64";

    inline result_type operator()(){
        return next();
    }

};

class Sfc32 {
    uint32_t a, b, c, counter;

    inline uint64_t next(){
        uint32_t tmp = a + b + counter++;
        a = b ^ (b >> 9);
        b = c + (c << 3);
        c = ((c << 21) | (c >> 11)) + tmp;
        return tmp;
    }

    explicit Sfc32(uint32_t seed){
        a = b = c = seed;
        counter = 1;

        for (int i = 0; i < 12; ++i){
            next();
        }
    }

    friend class Factory;

public:
    using result_type = uint32_t;
    static constexpr uint32_t min() { return 0; }
    static constexpr uint32_t max() { return UINT32_MAX; }
    static constexpr const char* id = "sfc32";

    inline result_type operator()(){
        return next();
    }

};

//----------------------------------------------------------------------------------------------------------------------

class L64X128M {
private:
    uint64_t s, a, x0, x1;

    friend class Factory;

    explicit L64X128M (uint64_t s, uint64_t a, uint64_t x0, uint64_t x1) : 
    s(s), a(a | 1), x0(x0), x1(x1) {
        
        if ((x0 | x1) == 0){
            uint64_t v = s + GOLDEN_RATIO_64;
            this->x0 = mixStafford13(v);
            this->x1 = mixStafford13(v + GOLDEN_RATIO_64);
        }
    }

    explicit L64X128M (uint64_t seed) : L64X128M (
        1,
        mixMurmur64(seed ^= SILVER_RATIO_64),
        mixStafford13(seed),
        mixStafford13(seed + GOLDEN_RATIO_64)
    ){}

    inline uint64_t next(){
        // Mixing function (lea64)
        uint64_t z = mixLea64(s + x0);
        // Update the LCG subgenerator
        s = L64XM_M * s + a;
        // Update the XBG subgenerator (xoroshiro128v1_0)
        x1 ^= x0;
        x0 = _rotl64(x0, 24) ^ x1 ^ (x1 << 16);
        x1 = _rotl64(x1, 37);
        // Return result
        return z;
    }

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "L64X128MixRandom";

    inline result_type operator()(){
        return next();
    }

    L64X128M split(){
        uint64_t next_s  = this->next();
        uint64_t next_a  = mixGamma(this->next()) << 1;
        uint64_t next_x0 = this->next();
        uint64_t next_x1 = this->next();
        
        return L64X128M(next_s, next_a, next_x0, next_x1);
    }
};

class L64X256M {
private:
    uint64_t s, a, x0, x1, x2, x3;

    friend class Factory;

    explicit L64X256M (uint64_t s, uint64_t a, uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3) : 
    s(s), a(a | 1), x0(x0), x1(x1), x2(x2), x3(x3) {
        
        if ((x0 | x1 | x2 | x3) == 0){
            uint64_t v = s + GOLDEN_RATIO_64;
            this->x0 = mixStafford13(v);
            this->x1 = mixStafford13(v += GOLDEN_RATIO_64);
            this->x2 = mixStafford13(v += GOLDEN_RATIO_64);
            this->x3 = mixStafford13(v + GOLDEN_RATIO_64);
        }
    }

    explicit L64X256M (uint64_t seed) : L64X256M (
        1,
        mixMurmur64(seed ^= SILVER_RATIO_64),
        mixStafford13(seed),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed + GOLDEN_RATIO_64)
    ){}

    inline uint64_t next(){
        // Mixing function (lea64)
        uint64_t z = mixLea64(s + x0);
        // Update the LCG subgenerator
        s = L64XM_M * s + a;
        // Update the XBG subgenerator 
        uint64_t t = x1 << 17;
        x2 ^= x0;
        x3 ^= x1;
        x1 ^= x2;
        x0 ^= x3;
        x2 ^= t;
        x3 = _rotl64(x3, 45);
        // Return result
        return z;
    }

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "L64X256MixRandom";

    inline result_type operator()(){
        return next();
    }

    L64X256M split(){
        uint64_t next_s  = this->next();
        uint64_t next_a  = mixGamma(this->next()) << 1;;
        uint64_t next_x0 = this->next();
        uint64_t next_x1 = this->next();
        uint64_t next_x2 = this->next();
        uint64_t next_x3 = this->next();
        
        return L64X256M(next_s, next_a, next_x0, next_x1, next_x2, next_x3);
    }

};

class L64X1024M {
private:
    uint64_t s, a;
    uint64_t x[16];
    int p = 15;

    friend class Factory;

    explicit L64X1024M (uint64_t s, uint64_t a, uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5, 
        uint64_t x6, uint64_t x7, uint64_t x8, uint64_t x9, uint64_t x10, uint64_t x11, uint64_t x12, uint64_t x13, uint64_t x14, uint64_t x15) : 
    s(s), a(a | 1), x{x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15} {
        
        if ((x0 | x1 | x2 | x3 | x4 | x5 | x6 | x7 | x8 | x9 | x10 | x11 | x12 | x13 | x14 | x15) == 0){
            uint64_t v = s;
            for (int i = 0; i < 16; ++i){
                x[i] = mixStafford13(v += GOLDEN_RATIO_64);
            }
        }
    }

    explicit L64X1024M (uint64_t seed) : L64X1024M (
        1,
        mixMurmur64(seed ^= SILVER_RATIO_64),
        mixStafford13(seed),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed + GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed + GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed + GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed + GOLDEN_RATIO_64)
    ){}

    inline uint64_t next(){
        const int q = p;
        const uint64_t s0 = x[p = (p + 1) & 15];
        uint64_t s15 = x[q];

        //mix
        const uint64_t z = mixLea64(s + s0);

        //update LCG
        s = L64XM_M * s + a;

        //update data
        s15 ^= s0;
        x[q] = _rotl64(s0, 25) ^ s15 ^ (s15 << 27);
        x[p] = _rotl64(s15, 36);

        return z;
    }

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "L64X1024MixRandom";

    inline result_type operator()(){
        return next();
    }

    L64X1024M split(){
        uint64_t next_s  = this->next();
        uint64_t next_a  = mixGamma(this->next()) << 1;;
        uint64_t next_x0 = this->next();
        uint64_t next_x1 = this->next();
        uint64_t next_x2 = this->next();
        uint64_t next_x3 = this->next();
        uint64_t next_x4 = this->next();
        uint64_t next_x5 = this->next();
        uint64_t next_x6 = this->next();
        uint64_t next_x7 = this->next();
        uint64_t next_x8 = this->next();
        uint64_t next_x9 = this->next();
        uint64_t next_x10 = this->next();
        uint64_t next_x11 = this->next();
        uint64_t next_x12 = this->next();
        uint64_t next_x13 = this->next();
        uint64_t next_x14 = this->next();
        uint64_t next_x15 = this->next();
        
        return L64X1024M(
            next_s, next_a, next_x0, next_x1, next_x2, next_x3,
            next_x4, next_x5, next_x6, next_x7,
            next_x8, next_x9, next_x10, next_x11,
            next_x12, next_x13, next_x14, next_x15
        );
    }

};

//----------------------------------------------------------------------------------------------------------------------

class L32X64M {
private:
    uint32_t s, a, x0, x1;

    friend class Factory;

    explicit L32X64M (uint32_t s, uint32_t a, uint32_t x0, uint32_t x1) : 
    s(s), a(a | 1), x0(x0), x1(x1) {
        
        if ((x0 | x1) == 0){
            uint32_t v = s + GOLDEN_RATIO_32;
            this->x0 = mixMurmur32(v);
            this->x1 = mixMurmur32(v + GOLDEN_RATIO_64);
        }
    }

    explicit L32X64M (uint32_t seed) : L32X64M (
        1,
        mixMurmur32(seed ^= SILVER_RATIO_32),
        mixLea32(seed),
        mixLea32(seed + GOLDEN_RATIO_32)
    ){}

    inline uint32_t next(){
        // Mixing function (lea64)
        uint32_t z = mixLea32(s + x0);
        // Update the LCG subgenerator
        s = L32XM_M * s + a;
        // Update the XBG subgenerator (xoroshiro128v1_0)
        x1 ^= x0;
        x0 = _rotl(x0, 26);
        x0 = x0 ^ x1 ^ (x1 << 9);
        x1 = _rotl(x1, 13);
        // Return result
        return z;
    }

public:
    using result_type = uint32_t;
    static constexpr uint32_t min() { return 0; }
    static constexpr uint32_t max() { return UINT32_MAX; }
    static constexpr const char* id = "L32X64MixRandom";

    inline result_type operator()(){
        return next();
    }

    L32X64M split(){
        uint64_t next_s  = this->next();
        uint64_t next_a  = mixGamma(this->next()) << 1;
        uint64_t next_x0 = this->next();
        uint64_t next_x1 = this->next();
        
        return L32X64M(next_s, next_a, next_x0, next_x1);
    }
};

//----------------------------------------------------------------------------------------------------------------------

class Xoroshiro128plusplus{

    uint64_t x0, x1;
    friend class Factory;

    explicit Xoroshiro128plusplus(uint64_t x0, uint64_t x1) : x0(x0), x1(x1) {
        if ((x0 | x1) == 0){
            this->x0 = GOLDEN_RATIO_64;
            this->x1 = SILVER_RATIO_64;
        }
    }

    explicit Xoroshiro128plusplus(uint64_t seed) : Xoroshiro128plusplus(
        mixStafford13(seed ^= SILVER_RATIO_64),
        mixStafford13(seed + GOLDEN_RATIO_64)
    ){}

    inline uint64_t next(){
        const uint64_t s0 = x0;
        uint64_t s1 = x1;

        //scrambler
        const uint64_t z = _rotl64(s0 + s1, 17) + s0;

        s1 ^= s0;
        x0 = _rotl64(s0, 49) ^ s1 ^ (s1 << 21);
        x1 = _rotl64(s1, 28);

        return z;
    }

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "Xoroshiro128++";

    inline result_type operator()(){
        return next();
    }

    Xoroshiro128plusplus split(){
        uint64_t k = this->next();

        uint64_t s0 = mixMurmur64(k ^ SILVER_RATIO_64);
        uint64_t s1 = mixMurmur64(k + GOLDEN_RATIO_64);

        return Xoroshiro128plusplus(s0, s1);
    }
};

class Xoshiro256plusplus{

    uint64_t x0, x1, x2, x3;

    friend class Factory;

    explicit Xoshiro256plusplus(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3) : x0(x0), x1(x1), x2(x2), x3(x3) {
        if ((x0 | x1 | x2 | x3) == 0){
            this->x0 = mixStafford13(x0 += GOLDEN_RATIO_64);
            this->x1 = x0 += SILVER_RATIO_64;
            this->x2 = x0 += SILVER_RATIO_64;
            this->x3 = x0 += SILVER_RATIO_64;
        }
    }

    explicit Xoshiro256plusplus(uint64_t seed) : Xoshiro256plusplus(
        mixStafford13(seed ^= SILVER_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed += GOLDEN_RATIO_64),
        mixStafford13(seed + GOLDEN_RATIO_64)
    ){}

    inline uint64_t next(){
        //scrambler
        const uint64_t z = _rotl64(x0 + x3, 23) + x0;

        uint64_t t = (x1 << 17);
        x2 ^= x0;          
        x3 ^= x1;          
        x1 ^= x2;         
        x0 ^= x3;      
        x2 ^= t;
        x3 = _rotl64(x3, 45);

        return z;
    }

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "Xoshiro256++";

    inline result_type operator()(){
        return next();
    }

    Xoshiro256plusplus split(){
        uint64_t k1 = this->next();
        uint64_t k2 = this->next();

        if (k1 == k2) k2 = mixLea64(k2 + GOLDEN_RATIO_64);

        uint64_t s0 = mixMurmur64(k1 ^ SILVER_RATIO_64);
        uint64_t s1 = mixMurmur64(k1 + GOLDEN_RATIO_64);
        uint64_t s2 = mixMurmur64(k2 ^ SILVER_RATIO_64);
        uint64_t s3 = mixMurmur64(k2 + GOLDEN_RATIO_64);
        
        return Xoshiro256plusplus(s0, s1, s2, s3);
    }
};

class PCG64DXSM{
    __uint128_t x;
    uint64_t c;

    // 0xda942042e4dd58b 
    static constexpr uint64_t half_M = 0xff37f1f758180525; //vigna suggestion

    friend class Factory;

    static inline uint64_t mixDXSM(__uint128_t seed){
        uint64_t hi = (uint64_t)(seed >> 64);
        uint64_t lo = (uint64_t)(seed | 1);

        hi ^= (hi >> 32);
        hi *= half_M;
        hi ^= (hi >> 48);
        hi *= lo;
        return hi;
    }

    explicit PCG64DXSM (uint64_t seed){
        c = (mixGamma(seed ^= SILVER_RATIO_64) << 1) | 1;
        uint64_t low_x = mixMurmur64(seed += GOLDEN_RATIO_64);
        uint64_t high_x = mixMurmur64(seed + GOLDEN_RATIO_64);
        
        x = ((__uint128_t) (high_x) << 64) | low_x;
    }

    inline uint64_t next(){
        __uint128_t z = x;
        //update lcg state
        x = x * half_M + c;
        return mixDXSM(z);
    } 

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "PCG64DXSM";

    inline result_type operator()(){
        return next();
    }

    PCG64DXSM split(){
        return PCG64DXSM(mixLea64(this->next()));
    }
};

class PCG64DXSphi{
    __uint128_t x;
    uint64_t c;

    friend class Factory;

    static inline uint64_t mixVigna(__uint128_t seed){
        uint64_t hi = (uint64_t)(seed >> 64);
        uint64_t lo = (uint64_t)(seed | 1);

        hi ^= (hi >> 32);
        hi *= GOLDEN_RATIO_64;
        hi ^= (hi >> 48);
        hi *= lo;
        return hi;
    }

    explicit PCG64DXSphi (uint64_t seed){
        c = (mixGamma(seed ^= SILVER_RATIO_64) << 1) | 1;
        uint64_t low_x = mixMurmur64(seed += GOLDEN_RATIO_64);
        uint64_t high_x = mixMurmur64(seed + GOLDEN_RATIO_64);
        
        x = ((__uint128_t) (high_x) << 64) | low_x;
    }

    inline uint64_t next(){
        __uint128_t z = x;
        //update lcg state
        x = x * (((__uint128_t)1 << 64) ^ 0xd605bbb58c8abbfdULL) + c;
        return mixVigna(z);
    } 

public:
    using result_type = uint64_t;
    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }
    static constexpr const char* id = "PCG64DXSphi";

    inline result_type operator()(){
        return next();
    }

    PCG64DXSphi split(){
        return PCG64DXSphi(mixLea64(this->next()));
    }
};

//----------------------------------------------------------------------------------------------------------------------

class Factory {
private:
    using default_tag = std::conditional_t<(sizeof(void*) == 8), l64x128m_t, l32x64m_t>;
    using fast_tag    = std::conditional_t<(sizeof(void*) == 8), sfc64_t,    sfc32_t>;
    using seed_t      = std::conditional_t<(sizeof(void*) == 8), uint64_t,   uint32_t>;
public:
    static auto set_rng(l64x128m_t, uint64_t seed = rd()) {
        return L64X128M(seed);
    }

    static auto set_rng(sfc64_t, uint64_t seed = rd()) {
        return Sfc64(seed);
    }

    static auto set_rng(sfc32_t, uint64_t seed = rd()) {
        return Sfc32(seed);
    }

    static auto set_rng(l64x256m_t, uint64_t seed = rd()) {
        return L64X256M(seed);
    }

    static auto set_rng(l64x1024m_t, uint64_t seed = rd()) {
        return L64X1024M(seed);
    }

    static auto set_rng(l32x64m_t, uint64_t seed = rd()) {
        return L32X64M(seed);
    }

    static auto set_rng(xoroshiro128_t, uint64_t seed = rd()) {
        return Xoroshiro128plusplus(seed);
    }

    static auto set_rng(xoshiro256_t, uint64_t seed = rd()) {
        return Xoshiro256plusplus(seed);
    }

    static auto set_rng(splitmix32_t, uint64_t seed = rd()) {
        return SplitMix32(seed);
    }

    static auto set_rng(splitmix64_t, uint64_t seed = rd()) {
        return SplitMix64(seed);
    }

    static auto set_rng(pcg64dxsm_t, uint64_t seed = rd()) {
        return PCG64DXSM(seed);
    }

    static auto set_rng(pcg64dxsphi_t, uint64_t seed = rd()) {
        return PCG64DXSphi(seed);
    }

    static auto default_rng(seed_t seed = static_cast<seed_t>(rd())) {
        return set_rng(default_tag{}, seed);
    }

    static auto fast_rng(seed_t seed = static_cast<seed_t>(rd())) {
        return set_rng(fast_tag{}, seed);
    }
    
};


};

#endif
