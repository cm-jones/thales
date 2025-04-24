// SPDX-License-Identifier: MIT

#pragma once

#include <limits>
#include <thales/models/black_scholes_model.hpp>

// Architecture detection
#if defined(__x86_64__) || defined(_M_X64) || defined(i386) ||                 \
    defined(__i386__) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h> // For AVX/AVX2 intrinsics
#define THALES_X86_OR_X64
#endif

// Prefetch constants
#ifndef THALES_PREFETCH_DISTANCE
#define THALES_PREFETCH_DISTANCE 8 // Prefetch 8 doubles ahead
#endif

// Cache line size
#ifndef THALES_CACHE_LINE_SIZE
#define THALES_CACHE_LINE_SIZE 64 // Most modern CPUs use 64-byte cache lines
#endif

// Alignment for vectorized operations
#ifndef THALES_VECTOR_ALIGNMENT
#define THALES_VECTOR_ALIGNMENT 32 // AVX/AVX2 requires 32-byte alignment
#endif

namespace thales {
namespace models {

/// @class BlackScholesVectorized
/// @brief SIMD-optimized implementation of the Black-Scholes option pricing
/// model.
///
/// @note This class provides vectorized implementations of Black-Scholes option
/// pricing calculations using AVX/AVX2 SIMD instructions, which can compute 4
/// option prices simultaneously for significant performance improvements.
class BlackScholesVectorized {
  public:
    /// @brief Calculate call option prices for multiple options using SIMD
    /// vectorization with prefetching and AVX2 optimizations
    /// @param S_array Array of underlying prices (must be aligned to
    /// THALES_VECTOR_ALIGNMENT)
    /// @param K_array Array of strike prices (must be aligned to
    /// THALES_VECTOR_ALIGNMENT)
    /// @param r_array Array of risk-free rates (must be aligned to
    /// THALES_VECTOR_ALIGNMENT)
    /// @param sigma_array Array of volatilities (must be aligned to
    /// THALES_VECTOR_ALIGNMENT)
    /// @param T_array Array of times to expiration (must be aligned to
    /// THALES_VECTOR_ALIGNMENT)
    /// @param result_array Output array for calculated prices (must be aligned
    /// to THALES_VECTOR_ALIGNMENT)
    /// @param n Number of options to price
    static void
    vectorized_call_price(const double *S_array, const double *K_array,
                          const double *r_array, const double *sigma_array,
                          const double *T_array, double *result_array, int n);

    /// @brief Calculate put option prices for multiple options using SIMD
    /// vectorization
    /// @param S_array Array of underlying prices
    /// @param K_array Array of strike prices
    /// @param r_array Array of risk-free rates
    /// @param sigma_array Array of volatilities
    /// @param T_array Array of times to expiration
    /// @param result_array Output array for calculated prices
    /// @param n Number of options to price
    static void
    vectorized_put_price(const double *S_array, const double *K_array,
                         const double *r_array, const double *sigma_array,
                         const double *T_array, double *result_array, int n);

    /// @brief Calculate call option deltas for multiple options using SIMD
    /// vectorization
    /// @param S_array Array of underlying prices
    /// @param K_array Array of strike prices
    /// @param r_array Array of risk-free rates
    /// @param sigma_array Array of volatilities
    /// @param T_array Array of times to expiration
    /// @param result_array Output array for calculated deltas
    /// @param n Number of options to calculate
    static void
    vectorized_call_delta(const double *S_array, const double *K_array,
                          const double *r_array, const double *sigma_array,
                          const double *T_array, double *result_array, int n);

    /// @brief Calculate gamma for multiple options using SIMD vectorization
    /// @param S_array Array of underlying prices
    /// @param K_array Array of strike prices
    /// @param r_array Array of risk-free rates
    /// @param sigma_array Array of volatilities
    /// @param T_array Array of times to expiration
    /// @param result_array Output array for calculated gammas
    /// @param n Number of options to calculate
    static void vectorized_gamma(const double *S_array, const double *K_array,
                                 const double *r_array,
                                 const double *sigma_array,
                                 const double *T_array, double *result_array,
                                 int n);

    /// @brief Check if the CPU supports AVX instructions
    /// @return true if AVX is supported, false otherwise
    static bool cpu_supports_avx();

    /// @brief Check if the CPU supports AVX2 instructions
    /// @return true if AVX2 is supported, false otherwise
    static bool cpu_supports_avx2();

  private:
#ifdef THALES_X86_OR_X64
    // Helper vectorized functions
    static __m256d normal_cdf_vector(__m256d x);
    static __m256d normal_pdf_vector(__m256d x);
    static __m256d d1_vector(__m256d S, __m256d K, __m256d r, __m256d sigma,
                             __m256d T);
    static __m256d d2_vector(__m256d S, __m256d K, __m256d r, __m256d sigma,
                             __m256d T);
#endif
};

} // namespace models
} // namespace thales
