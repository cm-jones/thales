// SPDX-License-Identifier: MIT

#include <thales/models/black_scholes_model_vectorized.hpp>

namespace thales {
namespace models {

void BlackScholesVectorized::vectorized_call_price(
    const double *S_array, const double *K_array, const double *r_array,
    const double *sigma_array, const double *T_array, double *result_array,
    int n) {
#ifdef THALES_X86_OR_X64
    if (!cpu_supports_avx2()) {
        if (!cpu_supports_avx()) {
            // Fallback to scalar implementation if neither AVX2 nor AVX
            // supported
            for (int i = 0; i < n; ++i) {
                result_array[i] = BlackScholesModel::call_price(
                    S_array[i], K_array[i], r_array[i], sigma_array[i],
                    T_array[i]);
            }
            return;
        }
    }

    const __m256d zero = _mm256_set1_pd(0.0);
    const __m256d neg_one = _mm256_set1_pd(-1.0);

    // Process 4 options at a time with prefetching
    for (int i = 0; i < n - 3; i += 4) {
        // Prefetch next iteration's data
        if (i + THALES_PREFETCH_DISTANCE < n) {
            _mm_prefetch(&S_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&K_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&r_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&sigma_array[i + THALES_PREFETCH_DISTANCE],
                         _MM_HINT_T0);
            _mm_prefetch(&T_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
        }

        // Load 4 sets of inputs using aligned loads
        __m256d S = _mm256_load_pd(&S_array[i]);
        __m256d K = _mm256_load_pd(&K_array[i]);
        __m256d r = _mm256_load_pd(&r_array[i]);
        __m256d sigma = _mm256_load_pd(&sigma_array[i]);
        __m256d T = _mm256_load_pd(&T_array[i]);

        // Check for expired options (T <= 0)
        __m256d expired_mask = _mm256_cmp_pd(T, zero, _CMP_LE_OQ);

        // For expired options, calculate intrinsic value max(0, S - K)
        __m256d S_minus_K = _mm256_sub_pd(S, K);
        __m256d intrinsic_value = _mm256_max_pd(zero, S_minus_K);

        // Calculate d1 and d2 for non-expired options
        __m256d d1 = d1_vector(S, K, r, sigma, T);
        __m256d d2 = d2_vector(S, K, r, sigma, T);

        // Calculate N(d1) and N(d2)
        __m256d N_d1 = normal_cdf_vector(d1);
        __m256d N_d2 = normal_cdf_vector(d2);

        // Calculate K * e^(-r*T)
        __m256d neg_r_T = _mm256_mul_pd(_mm256_mul_pd(neg_one, r), T);
        __m256d discount = _mm256_exp_pd(neg_r_T);
        __m256d discounted_K = _mm256_mul_pd(K, discount);

        // Calculate option price: S * N(d1) - K * e^(-r*T) * N(d2)
        __m256d term1 = _mm256_mul_pd(S, N_d1);
        __m256d term2 = _mm256_mul_pd(discounted_K, N_d2);
        __m256d option_price = _mm256_sub_pd(term1, term2);

        // Select intrinsic value for expired options, normal price otherwise
        __m256d result =
            _mm256_blendv_pd(option_price, intrinsic_value, expired_mask);

        // Store the results
        _mm256_storeu_pd(&result_array[i], result);
    }

    // Handle remaining elements
    for (int i = (n / 4) * 4; i < n; ++i) {
        result_array[i] = BlackScholesModel::call_price(
            S_array[i], K_array[i], r_array[i], sigma_array[i], T_array[i]);
    }
#else
    // Non-x86/x64 architecture: use scalar implementation
    for (int i = 0; i < n; ++i) {
        result_array[i] = BlackScholesModel::call_price(
            S_array[i], K_array[i], r_array[i], sigma_array[i], T_array[i]);
    }
#endif
}

void BlackScholesVectorized::vectorized_put_price(const double *S_array,
                                                  const double *K_array,
                                                  const double *r_array,
                                                  const double *sigma_array,
                                                  const double *T_array,
                                                  double *result_array, int n) {
#ifdef THALES_X86_OR_X64
    if (!cpu_supports_avx2()) {
        if (!cpu_supports_avx()) {
            // Fallback to scalar implementation if neither AVX2 nor AVX
            // supported
            for (int i = 0; i < n; ++i) {
                result_array[i] = BlackScholesModel::put_price(
                    S_array[i], K_array[i], r_array[i], sigma_array[i],
                    T_array[i]);
            }
            return;
        }
    }

    const __m256d zero = _mm256_set1_pd(0.0);
    const __m256d neg_one = _mm256_set1_pd(-1.0);

    // Process 4 options at a time with prefetching
    for (int i = 0; i < n - 3; i += 4) {
        // Prefetch next iteration's data
        if (i + THALES_PREFETCH_DISTANCE < n) {
            _mm_prefetch(&S_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&K_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&r_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&sigma_array[i + THALES_PREFETCH_DISTANCE],
                         _MM_HINT_T0);
            _mm_prefetch(&T_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
        }

        // Load 4 sets of inputs using aligned loads
        __m256d S = _mm256_load_pd(&S_array[i]);
        __m256d K = _mm256_load_pd(&K_array[i]);
        __m256d r = _mm256_load_pd(&r_array[i]);
        __m256d sigma = _mm256_load_pd(&sigma_array[i]);
        __m256d T = _mm256_load_pd(&T_array[i]);

        // Check for expired options (T <= 0)
        __m256d expired_mask = _mm256_cmp_pd(T, zero, _CMP_LE_OQ);

        // For expired options, calculate intrinsic value max(0, K - S)
        __m256d K_minus_S = _mm256_sub_pd(K, S);
        __m256d intrinsic_value = _mm256_max_pd(zero, K_minus_S);

        // Calculate d1 and d2 for non-expired options
        __m256d d1 = d1_vector(S, K, r, sigma, T);
        __m256d d2 = d2_vector(S, K, r, sigma, T);

        // Calculate N(-d1) and N(-d2)
        __m256d neg_d1 = _mm256_mul_pd(d1, neg_one);
        __m256d neg_d2 = _mm256_mul_pd(d2, neg_one);
        __m256d N_neg_d1 = normal_cdf_vector(neg_d1);
        __m256d N_neg_d2 = normal_cdf_vector(neg_d2);

        // Calculate K * e^(-r*T)
        __m256d neg_r_T = _mm256_mul_pd(_mm256_mul_pd(neg_one, r), T);
        __m256d discount = _mm256_exp_pd(neg_r_T);
        __m256d discounted_K = _mm256_mul_pd(K, discount);

        // Calculate option price: K * e^(-r*T) * N(-d2) - S * N(-d1)
        __m256d term1 = _mm256_mul_pd(discounted_K, N_neg_d2);
        __m256d term2 = _mm256_mul_pd(S, N_neg_d1);
        __m256d option_price = _mm256_sub_pd(term1, term2);

        // Select intrinsic value for expired options, normal price otherwise
        __m256d result =
            _mm256_blendv_pd(option_price, intrinsic_value, expired_mask);

        // Store the results
        _mm256_storeu_pd(&result_array[i], result);
    }

    // Handle remaining elements
    for (int i = (n / 4) * 4; i < n; ++i) {
        result_array[i] = BlackScholesModel::put_price(
            S_array[i], K_array[i], r_array[i], sigma_array[i], T_array[i]);
    }
#else
    // Non-x86/x64 architecture: use scalar implementation
    for (int i = 0; i < n; ++i) {
        result_array[i] = BlackScholesModel::put_price(
            S_array[i], K_array[i], r_array[i], sigma_array[i], T_array[i]);
    }
#endif
}

void BlackScholesVectorized::vectorized_call_delta(
    const double *S_array, const double *K_array, const double *r_array,
    const double *sigma_array, const double *T_array, double *result_array,
    int n) {
#ifdef THALES_X86_OR_X64
    if (!cpu_supports_avx2()) {
        if (!cpu_supports_avx()) {
            // Fallback to scalar implementation if neither AVX2 nor AVX
            // supported
            for (int i = 0; i < n; ++i) {
                result_array[i] = BlackScholesModel::call_delta(
                    S_array[i], K_array[i], r_array[i], sigma_array[i],
                    T_array[i]);
            }
            return;
        }
    }

    const __m256d zero = _mm256_set1_pd(0.0);
    const __m256d one = _mm256_set1_pd(1.0);

    // Process 4 options at a time with prefetching
    for (int i = 0; i < n - 3; i += 4) {
        // Prefetch next iteration's data
        if (i + THALES_PREFETCH_DISTANCE < n) {
            _mm_prefetch(&S_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&K_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&r_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&sigma_array[i + THALES_PREFETCH_DISTANCE],
                         _MM_HINT_T0);
            _mm_prefetch(&T_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
        }

        // Load 4 sets of inputs using aligned loads
        __m256d S = _mm256_load_pd(&S_array[i]);
        __m256d K = _mm256_load_pd(&K_array[i]);
        __m256d r = _mm256_load_pd(&r_array[i]);
        __m256d sigma = _mm256_load_pd(&sigma_array[i]);
        __m256d T = _mm256_load_pd(&T_array[i]);

        // Check for expired options (T <= 0)
        __m256d expired_mask = _mm256_cmp_pd(T, zero, _CMP_LE_OQ);

        // For expired options, delta is either 1.0 (if S > K) or 0.0 (if S <=
        // K)
        __m256d S_gt_K = _mm256_cmp_pd(S, K, _CMP_GT_OQ);
        __m256d expired_delta = _mm256_and_pd(one, S_gt_K);

        // Calculate d1 for non-expired options
        __m256d d1 = d1_vector(S, K, r, sigma, T);

        // Calculate N(d1)
        __m256d delta = normal_cdf_vector(d1);

        // Select appropriate delta based on expiration
        __m256d result = _mm256_blendv_pd(delta, expired_delta, expired_mask);

        // Store the results
        _mm256_storeu_pd(&result_array[i], result);
    }

    // Handle remaining elements
    for (int i = (n / 4) * 4; i < n; ++i) {
        result_array[i] = BlackScholesModel::call_delta(
            S_array[i], K_array[i], r_array[i], sigma_array[i], T_array[i]);
    }
#else
    // Non-x86/x64 architecture: use scalar implementation
    for (int i = 0; i < n; ++i) {
        result_array[i] = BlackScholesModel::call_delta(
            S_array[i], K_array[i], r_array[i], sigma_array[i], T_array[i]);
    }
#endif
}

void BlackScholesVectorized::vectorized_gamma(const double *S_array,
                                              const double *K_array,
                                              const double *r_array,
                                              const double *sigma_array,
                                              const double *T_array,
                                              double *result_array, int n) {
#ifdef THALES_X86_OR_X64
    if (!cpu_supports_avx2()) {
        if (!cpu_supports_avx()) {
            // Fallback to scalar implementation if neither AVX2 nor AVX
            // supported
            for (int i = 0; i < n; ++i) {
                result_array[i] =
                    BlackScholesModel::gamma(S_array[i], K_array[i], r_array[i],
                                             sigma_array[i], T_array[i]);
            }
            return;
        }
    }

    const __m256d zero = _mm256_set1_pd(0.0);

    // Process 4 options at a time with prefetching
    for (int i = 0; i < n - 3; i += 4) {
        // Prefetch next iteration's data
        if (i + THALES_PREFETCH_DISTANCE < n) {
            _mm_prefetch(&S_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&K_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&r_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
            _mm_prefetch(&sigma_array[i + THALES_PREFETCH_DISTANCE],
                         _MM_HINT_T0);
            _mm_prefetch(&T_array[i + THALES_PREFETCH_DISTANCE], _MM_HINT_T0);
        }

        // Load 4 sets of inputs using aligned loads
        __m256d S = _mm256_load_pd(&S_array[i]);
        __m256d K = _mm256_load_pd(&K_array[i]);
        __m256d r = _mm256_load_pd(&r_array[i]);
        __m256d sigma = _mm256_load_pd(&sigma_array[i]);
        __m256d T = _mm256_load_pd(&T_array[i]);

        // Check for expired options or invalid inputs
        __m256d valid_mask =
            _mm256_and_pd(_mm256_cmp_pd(T, zero, _CMP_GT_OQ),
                          _mm256_cmp_pd(sigma, zero, _CMP_GT_OQ));

        // Calculate d1 for valid inputs
        __m256d d1 = d1_vector(S, K, r, sigma, T);

        // Calculate N'(d1) = normal PDF of d1
        __m256d pdf_d1 = normal_pdf_vector(d1);

        // Calculate denominator: S * sigma * sqrt(T)
        __m256d sqrt_T = _mm256_sqrt_pd(T);
        __m256d denom1 = _mm256_mul_pd(S, sigma);
        __m256d denominator = _mm256_mul_pd(denom1, sqrt_T);

        // Calculate gamma = N'(d1) / (S * sigma * sqrt(T))
        __m256d gamma = _mm256_div_pd(pdf_d1, denominator);

        // Set gamma to zero for expired options or invalid inputs
        __m256d result = _mm256_and_pd(gamma, valid_mask);

        // Store the results
        _mm256_storeu_pd(&result_array[i], result);
    }

    // Handle remaining elements
    for (int i = (n / 4) * 4; i < n; ++i) {
        result_array[i] = BlackScholesModel::gamma(
            S_array[i], K_array[i], r_array[i], sigma_array[i], T_array[i]);
    }
#else
    // Non-x86/x64 architecture: use scalar implementation
    for (int i = 0; i < n; ++i) {
        result_array[i] = BlackScholesModel::gamma(
            S_array[i], K_array[i], r_array[i], sigma_array[i], T_array[i]);
    }
#endif
}

#ifdef THALES_X86_OR_X64
// Helper function to compute normal CDF for vector of values using
// approximation
__m256d BlackScholesVectorized::normal_cdf_vector(__m256d x) {
    // Constants for approximation
    const __m256d b1 = _mm256_set1_pd(0.31938153);
    const __m256d b2 = _mm256_set1_pd(-0.356563782);
    const __m256d b3 = _mm256_set1_pd(1.781477937);
    const __m256d b4 = _mm256_set1_pd(-1.821255978);
    const __m256d b5 = _mm256_set1_pd(1.330274429);
    const __m256d p = _mm256_set1_pd(0.2316419);
    const __m256d c = _mm256_set1_pd(0.39894228);
    const __m256d zero = _mm256_set1_pd(0.0);
    const __m256d one = _mm256_set1_pd(1.0);
    const __m256d half = _mm256_set1_pd(0.5);

    // Create mask for positive values
    __m256d mask = _mm256_cmp_pd(x, zero, _CMP_GE_OQ);

    // Calculate absolute value of x
    __m256d abs_x = _mm256_max_pd(x, _mm256_sub_pd(zero, x));

    // t = 1.0 / (1.0 + p * |x|)
    __m256d px = _mm256_mul_pd(p, abs_x);
    __m256d t = _mm256_div_pd(one, _mm256_add_pd(one, px));

    // exp(-x²/2) part
    __m256d x_squared = _mm256_mul_pd(x, x);
    __m256d x_squared_half = _mm256_mul_pd(x_squared, half);
    __m256d exp_val = _mm256_exp_pd(_mm256_sub_pd(zero, x_squared_half));

    // Compute polynomial
    __m256d term5 = _mm256_mul_pd(t, b5);
    __m256d term4 = _mm256_add_pd(b4, term5);
    __m256d term3 = _mm256_add_pd(b3, _mm256_mul_pd(t, term4));
    __m256d term2 = _mm256_add_pd(b2, _mm256_mul_pd(t, term3));
    __m256d term1 = _mm256_add_pd(b1, _mm256_mul_pd(t, term2));
    __m256d poly = _mm256_mul_pd(t, term1);

    // Calculate final values
    __m256d cexp_t_poly =
        _mm256_mul_pd(c, _mm256_mul_pd(exp_val, _mm256_mul_pd(t, poly)));
    __m256d result =
        _mm256_blendv_pd(cexp_t_poly, _mm256_sub_pd(one, cexp_t_poly), mask);

    return result;
}

// Helper function for normal probability density function
__m256d BlackScholesVectorized::normal_pdf_vector(__m256d x) {
    const __m256d one_over_sqrt_2pi = _mm256_set1_pd(0.3989422804014327);
    const __m256d zero = _mm256_set1_pd(0.0);
    const __m256d half = _mm256_set1_pd(0.5);

    // -x²/2
    __m256d x_squared = _mm256_mul_pd(x, x);
    __m256d neg_x_squared_half =
        _mm256_mul_pd(x_squared, _mm256_sub_pd(zero, half));

    // e^(-x²/2)
    __m256d exp_val = _mm256_exp_pd(neg_x_squared_half);

    // (1/√(2π)) * e^(-x²/2)
    return _mm256_mul_pd(one_over_sqrt_2pi, exp_val);
}

// Helper function to compute d1 parameter
__m256d BlackScholesVectorized::d1_vector(__m256d S, __m256d K, __m256d r,
                                          __m256d sigma, __m256d T) {
    const __m256d zero = _mm256_set1_pd(0.0);
    const __m256d half = _mm256_set1_pd(0.5);

    // Handle edge cases where T or sigma is zero or negative
    __m256d valid_mask = _mm256_and_pd(_mm256_cmp_pd(T, zero, _CMP_GT_OQ),
                                       _mm256_cmp_pd(sigma, zero, _CMP_GT_OQ));

    // Calculate log(S/K)
    __m256d S_div_K = _mm256_div_pd(S, K);
    __m256d log_S_K = _mm256_log_pd(S_div_K);

    // Calculate (r + 0.5 * sigma²)
    __m256d sigma_squared = _mm256_mul_pd(sigma, sigma);
    __m256d half_sigma_squared = _mm256_mul_pd(half, sigma_squared);
    __m256d r_plus_half_sigma_squared = _mm256_add_pd(r, half_sigma_squared);

    // Multiply by T
    __m256d rt = _mm256_mul_pd(r_plus_half_sigma_squared, T);

    // Add log(S/K)
    __m256d numerator = _mm256_add_pd(log_S_K, rt);

    // Calculate denominator (sigma * sqrt(T))
    __m256d sqrt_T = _mm256_sqrt_pd(T);
    __m256d denominator = _mm256_mul_pd(sigma, sqrt_T);

    // Calculate d1 = numerator / denominator
    __m256d d1 = _mm256_div_pd(numerator, denominator);

    // Return NaN for invalid inputs
    __m256d nan_vector =
        _mm256_set1_pd(std::numeric_limits<double>::quiet_NaN());
    return _mm256_blendv_pd(nan_vector, d1, valid_mask);
}

// Helper function to compute d2 parameter
__m256d BlackScholesVectorized::d2_vector(__m256d S, __m256d K, __m256d r,
                                          __m256d sigma, __m256d T) {
    __m256d d1 = d1_vector(S, K, r, sigma, T);
    __m256d sqrt_T = _mm256_sqrt_pd(T);
    __m256d sigma_sqrt_T = _mm256_mul_pd(sigma, sqrt_T);
    return _mm256_sub_pd(d1, sigma_sqrt_T);
}
#endif

bool BlackScholesVectorized::cpu_supports_avx() {
#ifdef THALES_X86_OR_X64
    int cpu_info[4];
    __cpuid(cpu_info, 1);
    return (cpu_info[2] & (1 << 28)) != 0; // Check AVX bit
#else
    return false;
#endif
}

bool BlackScholesVectorized::cpu_supports_avx2() {
#ifdef THALES_X86_OR_X64
    int cpu_info[4];
    __cpuid(cpu_info, 7);
    return (cpu_info[1] & (1 << 5)) != 0; // Check AVX2 bit
#else
    return false;
#endif
}

} // namespace models
} // namespace thales
