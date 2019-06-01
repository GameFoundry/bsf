#   Copyright (C) 2012-2013  Povilas Kanapickas <povilas@radix.lt>
#
#   Distributed under the Boost Software License, Version 1.0.
#       (See accompanying file LICENSE_1_0.txt or copy at
#           http://www.boost.org/LICENSE_1_0.txt)

include(CheckCXXSourceRuns)
include(CheckCXXSourceCompiles)

# ------------------------------------------------------------------------------
# Compiler checks (internal)
set(SIMDPP_GCC 0)
set(SIMDPP_CLANG 0)
set(SIMDPP_MSVC 0)
set(SIMDPP_INTEL 0)
set(SIMDPP_MSVC_INTEL 0)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(SIMDPP_CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Apple")
    set(SIMDPP_CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(SIMDPP_GCC 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if(MSVC)
        set(SIMDPP_MSVC_INTEL 1)
    else()
        set(SIMDPP_INTEL 1)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(SIMDPP_MSVC 1)
else()
    message(FATAL_ERROR "Compiler '${CMAKE_CXX_COMPILER_ID}' not recognized")
endif()

# ------------------------------------------------------------------------------
# Architecture descriptions (internal)
#
# Each architecture has the following information specific to it:
#  - SIMDPP_${ARCH}_TEST_CODE: source code snippet that uses functionality
#       from that arch. Used for @c check_cxx_source_runs macro.
#       We are taking extra care to confuse the compiler so that it does not
#       optimize things out. Nowadays compilers have good sense of when things
#       don't have side effects and will see through simple obfuscation
#       patterns.
#  - SIMDPP_${ARCH}_CXX_FLAGS: compiler flags that are needed for compilation.
#  - SIMDPP_${ARCH}_DEFINE: defines the macro that is needed to enable the
#       specific instruction set within the library.
#  - SIMDPP_${ARCH}_SUFFIX: defines a suffix to append to the filename of the
#       source file specific to this architecture.
#
# Three lists are created:
#
#  - SIMDPP_ARCHS_PRI - primary architectures.
#  - SIMDPP_ARCHS_SEC - secondary architectures. Effectively equivalent to one
#       of the primary architectures, just different instructions are generated
#       in specific scenarios.
#  - SIMDPP_ARCHS - all architectures
#

list(APPEND SIMDPP_ARCHS_PRI "X86_SSE2")
if(SIMDPP_CLANG OR SIMDPP_GCC OR SIMDPP_INTEL)
    set(SIMDPP_X86_SSE2_CXX_FLAGS "-msse2")
elseif(SIMDPP_MSVC OR SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_SSE2_CXX_FLAGS "/arch:SSE2")
endif()
set(SIMDPP_X86_SSE2_DEFINE "SIMDPP_ARCH_X86_SSE2")
set(SIMDPP_X86_SSE2_SUFFIX "-x86_sse2")
set(SIMDPP_X86_SSE2_TEST_CODE
    "#include <emmintrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            __m128i align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m128i one = _mm_load_si128((__m128i*)p);
        one = _mm_or_si128(one, one);
        _mm_store_si128((__m128i*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_SSE3")
if(SIMDPP_CLANG OR SIMDPP_GCC OR SIMDPP_INTEL)
    set(SIMDPP_X86_SSE3_CXX_FLAGS "-msse3")
elseif(SIMDPP_MSVC)
    set(SIMDPP_X86_SSE3_CXX_FLAGS "/arch:SSE2")
elseif(SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_SSE3_CXX_FLAGS "/arch:SSE3")
endif()
set(SIMDPP_X86_SSE3_DEFINE "SIMDPP_ARCH_X86_SSE3")
set(SIMDPP_X86_SSE3_SUFFIX "-x86_sse3")
set(SIMDPP_X86_SSE3_TEST_CODE
    "#include <pmmintrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            __m128 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m128 one = _mm_load_ps((float*)p);
        one = _mm_hadd_ps(one, one);
        _mm_store_ps((float*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_SSSE3")
if(SIMDPP_CLANG OR SIMDPP_GCC OR SIMDPP_INTEL)
    set(SIMDPP_X86_SSSE3_CXX_FLAGS "-mssse3")
elseif(SIMDPP_MSVC)
    set(SIMDPP_X86_SSSE3_CXX_FLAGS "/arch:SSE2")
elseif(SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_SSSE3_CXX_FLAGS "/arch:SSSE3")
endif()
set(SIMDPP_X86_SSSE3_DEFINE "SIMDPP_ARCH_X86_SSSE3")
set(SIMDPP_X86_SSSE3_SUFFIX "-x86_ssse3")
set(SIMDPP_X86_SSSE3_TEST_CODE
    "#include <tmmintrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            __m128i align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m128i one = _mm_load_si128((__m128i*)p);
        one = _mm_abs_epi8(one);
        _mm_store_si128((__m128i*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_SSE4_1")
if(SIMDPP_CLANG OR SIMDPP_GCC OR SIMDPP_INTEL)
    set(SIMDPP_X86_SSE4_1_CXX_FLAGS "-msse4.1")
elseif(SIMDPP_MSVC)
    set(SIMDPP_X86_SSE4_1_CXX_FLAGS "/arch:SSE2")
elseif(SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_SSE4_1_CXX_FLAGS "/arch:SSE4.1")
endif()
set(SIMDPP_X86_SSE4_1_DEFINE "SIMDPP_ARCH_X86_SSE4_1")
set(SIMDPP_X86_SSE4_1_SUFFIX "-x86_sse4_1")
set(SIMDPP_X86_SSE4_1_TEST_CODE
    "#include <smmintrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            __m128i align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m128i one = _mm_load_si128((__m128i*)p);
        one = _mm_cvtepi16_epi32(one);
        _mm_store_si128((__m128i*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_POPCNT_INSN")
if(SIMDPP_CLANG OR SIMDPP_GCC OR SIMDPP_INTEL)
    set(SIMDPP_X86_POPCNT_INSN_CXX_FLAGS "-mssse3 -mpopcnt")
elseif(SIMDPP_MSVC)
    set(SIMDPP_X86_POPCNT_INSN_CXX_FLAGS "/arch:SSE4.2")
elseif(SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_POPCNT_INSN_CXX_FLAGS "/arch:SSE4.2")
endif()
set(SIMDPP_X86_POPCNT_INSN_DEFINE "SIMDPP_ARCH_X86_POPCNT_INSN")
set(SIMDPP_X86_POPCNT_INSN_SUFFIX "-x86_popcnt")
set(SIMDPP_X86_POPCNT_INSN_TEST_CODE
    "#include <nmmintrin.h>
    #include <iostream>
    unsigned* prevent_optimization(unsigned* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        unsigned* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        unsigned data;
        unsigned* p = &data;
        p = prevent_optimization(p);
        *p = _mm_popcnt_u32(*p);
        p = prevent_optimization(p);
    }"
)
###

list(APPEND SIMDPP_ARCHS_PRI "X86_AVX")
if(SIMDPP_CLANG OR SIMDPP_GCC OR SIMDPP_INTEL)
    set(SIMDPP_X86_AVX_CXX_FLAGS "-mavx")
elseif(SIMDPP_MSVC OR SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_AVX_CXX_FLAGS "/arch:AVX")
endif()
set(SIMDPP_X86_AVX_DEFINE "SIMDPP_ARCH_X86_AVX")
set(SIMDPP_X86_AVX_SUFFIX "-x86_avx")
set(SIMDPP_X86_AVX_TEST_CODE
    "#include <immintrin.h>
    #include <iostream>
    #if (__clang_major__ == 3) && (__clang_minor__ == 6)
    #error Not supported. See simdpp/detail/workarounds.h
    #endif
    #if (__GNUC__ == 4) && (__GNUC_MINOR__ == 4) && !defined(__INTEL_COMPILER) && !defined(__clang__)
    #error Not supported. See simdpp/detail/workarounds.h
    #endif
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[32];
            __m256 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m256 one = _mm256_load_ps((float*)p);
        one = _mm256_add_ps(one, one);
        _mm256_store_ps((float*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_AVX2")
if(SIMDPP_CLANG OR SIMDPP_GCC)
    set(SIMDPP_X86_AVX2_CXX_FLAGS "-mavx2")
elseif(SIMDPP_INTEL)
    set(SIMDPP_X86_AVX2_CXX_FLAGS "-xCORE-AVX2")
elseif(SIMDPP_MSVC)
    set(SIMDPP_X86_AVX2_CXX_FLAGS "/arch:AVX")
elseif(SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_AVX2_CXX_FLAGS "/arch:CORE-AVX2")
endif()
set(SIMDPP_X86_AVX2_DEFINE "SIMDPP_ARCH_X86_AVX2")
set(SIMDPP_X86_AVX2_SUFFIX "-x86_avx2")
set(SIMDPP_X86_AVX2_TEST_CODE
    "#include <immintrin.h>
    #include <iostream>
    #if (__clang_major__ == 3) && (__clang_minor__ == 6)
    #error Not supported. See simdpp/detail/workarounds.h
    #endif
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[32];
            __m256 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m256i one = _mm256_load_si256((__m256i*)p);
        one = _mm256_or_si256(one, one);
        _mm256_store_si256((__m256i*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_FMA3")
if(SIMDPP_CLANG OR SIMDPP_GCC)
    set(SIMDPP_X86_FMA3_CXX_FLAGS "-mfma")
elseif(SIMDPP_INTEL)
    set(SIMDPP_X86_FMA3_CXX_FLAGS "-xCORE-AVX2")
elseif(SIMDPP_MSVC)
    set(SIMDPP_X86_FMA3_CXX_FLAGS "/arch:AVX")
elseif(SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_FMA3_CXX_FLAGS "/arch:CORE-AVX2")
endif()
set(SIMDPP_X86_FMA3_DEFINE "SIMDPP_ARCH_X86_FMA3")
set(SIMDPP_X86_FMA3_SUFFIX "-x86_fma3")
set(SIMDPP_X86_FMA3_TEST_CODE
    "#include <immintrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            __m128 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m128 one = _mm_load_ps((float*)p);
        one = _mm_fmadd_ps(one, one, one);
        _mm_store_ps((float*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_FMA4")
if(SIMDPP_CLANG OR SIMDPP_GCC)
    # intel does not support FMA4
    set(SIMDPP_X86_FMA4_CXX_FLAGS "-mfma4")
elseif(SIMDPP_MSVC OR SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_FMA4_CXX_FLAGS "/arch:AVX")
endif()
set(SIMDPP_X86_FMA4_DEFINE "SIMDPP_ARCH_X86_FMA4")
set(SIMDPP_X86_FMA4_SUFFIX "-x86_fma4")
set(SIMDPP_X86_FMA4_TEST_CODE
    "#include <x86intrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            __m128 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m128 one = _mm_load_ps((float*)p);
        one = _mm_macc_ps(one, one, one);
        _mm_store_ps((float*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_XOP")
if(SIMDPP_CLANG OR SIMDPP_GCC)
    set(SIMDPP_X86_XOP_CXX_FLAGS "-mxop")
endif()
set(SIMDPP_X86_XOP_DEFINE "SIMDPP_ARCH_X86_XOP")
set(SIMDPP_X86_XOP_SUFFIX "-x86_xop")
set(SIMDPP_X86_XOP_TEST_CODE
    "#include <x86intrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            __m128i align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m128i one = _mm_load_si128((__m128i*)p);
        one = _mm_cmov_si128(one, one, one);
        one = _mm_comeq_epi64(one, one);
        _mm_store_si128((__m128i*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_AVX512F")
if(SIMDPP_CLANG OR SIMDPP_GCC)
    set(SIMDPP_X86_AVX512F_CXX_FLAGS "-mavx512f -O1")
elseif(SIMDPP_INTEL)
    set(SIMDPP_X86_AVX512F_CXX_FLAGS "-xCOMMON-AVX512")
elseif(SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_AVX512F_CXX_FLAGS "/arch:COMMON-AVX512")
else()
    #unsupported on MSVC
endif()
set(SIMDPP_X86_AVX512F_DEFINE "SIMDPP_ARCH_X86_AVX512F")
set(SIMDPP_X86_AVX512F_SUFFIX "-x86_avx512f")
set(SIMDPP_X86_AVX512F_TEST_CODE
    "#include <immintrin.h>
    #include <iostream>
    #if defined(__GNUC__) && (__GNUC__ < 6) && !defined(__INTEL_COMPILER) && !defined(__clang__)
    #error GCC 5.x and older are not supported on AVX512F. See simdpp/detail/workarounds.h
    #endif
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[64];
            __m512 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m512 f = _mm512_load_ps((float*)p);
        p = prevent_optimization(p);
        __m512i i = _mm512_load_epi32((__m512i*)p);
        p = prevent_optimization(p);
        f = _mm512_add_ps(f, f);
        // MSVC 2017 miss this
        i = _mm512_or_epi32(i, i);
        f = _mm512_ceil_ps(f);
        // ICE on various versions of Clang trying to select palignr
        __m512i i2 = _mm512_load_epi32((__m512i*)p);
        __m512i ap = _mm512_alignr_epi32(i, i, 2);
        i = _mm512_mask_alignr_epi32(ap, 0xcccc, i2, i2, 14);
        p = prevent_optimization(p);
        _mm512_store_ps((float*)p, f);
        p = prevent_optimization(p);
        _mm512_store_epi32((void*)p, i);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_AVX512BW")
if(SIMDPP_CLANG OR SIMDPP_GCC)
    set(SIMDPP_X86_AVX512BW_CXX_FLAGS "-mavx512bw")
    #unsupported on MSVC
elseif(SIMDPP_INTEL)
    set(SIMDPP_X86_AVX512BW_CXX_FLAGS "-xCORE-AVX512")
elseif(SIMDPP_MSVC_INTEL)
    set(SIMDPP_X86_AVX512BW_CXX_FLAGS "/arch:CORE-AVX512")
endif()
set(SIMDPP_X86_AVX512BW_DEFINE "SIMDPP_ARCH_X86_AVX512BW")
set(SIMDPP_X86_AVX512BW_SUFFIX "-x86_avx512bw")
set(SIMDPP_X86_AVX512BW_TEST_CODE
    "#include <immintrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[64];
            __m512i align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m512i i = _mm512_load_si512((void*)p);
        i = _mm512_add_epi16(i, i); // only in AVX-512BW
        _mm512_store_si512((void*)p, i);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_AVX512DQ")
if(SIMDPP_CLANG OR SIMDPP_GCC OR SIMDPP_INTEL)
    set(SIMDPP_X86_AVX512DQ_CXX_FLAGS "-mavx512dq")
    #unsupported on MSVC
endif()
set(SIMDPP_X86_AVX512DQ_DEFINE "SIMDPP_ARCH_X86_AVX512DQ")
set(SIMDPP_X86_AVX512DQ_SUFFIX "-x86_avx512dq")
set(SIMDPP_X86_AVX512DQ_TEST_CODE
    "#include <immintrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[64];
            __m512 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m512 f = _mm512_load_ps((float*)p);
        f = _mm512_and_ps(f, f); // only in AVX512-DQ
        _mm512_store_ps((float*)p, f);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "X86_AVX512VL")
if(SIMDPP_CLANG OR SIMDPP_GCC OR SIMDPP_INTEL)
    set(SIMDPP_X86_AVX512VL_CXX_FLAGS "-mavx512vl")
    #unsupported on MSVC
endif()
set(SIMDPP_X86_AVX512VL_DEFINE "SIMDPP_ARCH_X86_AVX512VL")
set(SIMDPP_X86_AVX512VL_SUFFIX "-x86_avx512vl")
set(SIMDPP_X86_AVX512VL_TEST_CODE
    "#if !defined(__APPLE__) && (__clang_major__ == 3)
    #error AVX512VL is not supported on clang 3.9 and earlier.
    #endif
    #include <immintrin.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            __m128 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        __m128 f = _mm_load_ps((float*)p);
        f = _mm_rcp14_ps(f); // only in AVX512-VL
        _mm_store_ps((float*)p, f);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "ARM_NEON")
if(SIMDPP_CLANG OR SIMDPP_GCC)
    set(SIMDPP_ARM_NEON_CXX_FLAGS "-mfpu=neon")
endif()
set(SIMDPP_ARM_NEON_DEFINE "SIMDPP_ARCH_ARM_NEON")
set(SIMDPP_ARM_NEON_SUFFIX "-arm_neon")
set(SIMDPP_ARM_NEON_TEST_CODE
    "#if defined(__clang_major__)
    #if (__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 3))
    #error NEON is not supported on clang 3.3 and earlier.
    #endif
    #endif
    #include <arm_neon.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            uint32x4_t align;
        };
        char* p = data;
        p = prevent_optimization(p);
        uint32x4_t one = vld1q_u32((uint32_t*)p);
        one = vaddq_u32(one, one);
        vst1q_u32((uint32_t*)p, one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_SEC "ARM_NEON_FLT_SP")
if(SIMDPP_CLANG OR SIMDPP_GCC)
    set(SIMDPP_ARM_NEON_FLT_SP_CXX_FLAGS "-mfpu=neon")
endif()
set(SIMDPP_ARM_NEON_FLT_SP_DEFINE "SIMDPP_ARCH_ARM_NEON_FLT_SP")
set(SIMDPP_ARM_NEON_FLT_SP_SUFFIX "-arm_neon_flt_sp")

list(APPEND SIMDPP_ARCHS_PRI "ARM64_NEON")
if(SIMDPP_CLANG)
    set(SIMDPP_ARM64_NEON_CXX_FLAGS "-arch arm64")
elseif(SIMDPP_GCC)
    set(SIMDPP_ARM64_NEON_CXX_FLAGS "-mcpu=generic+simd")
endif()
set(SIMDPP_ARM64_NEON_DEFINE "SIMDPP_ARCH_ARM_NEON")
set(SIMDPP_ARM64_NEON_SUFFIX "-arm64_neon")
set(SIMDPP_ARM64_NEON_TEST_CODE
    "#include <arm_neon.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            uint32x4_t align;
        };
        char* p = data;
        p = prevent_optimization(p);
        uint32x4_t one = vld1q_u32((uint32_t*)(*p));
        one = vaddq_u32(one, one);
        // GCC 4.8 misses a subset of functions
        one = vdupq_laneq_u32(one, 1);
        vst1q_u32((uint32_t*)(*p), one);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "MIPS_MSA")
set(SIMDPP_MIPS_MSA_CXX_FLAGS "-mips64r5 -mmsa -mhard-float -mfp64 -mnan=legacy")
set(SIMDPP_MIPS_MSA_DEFINE "SIMDPP_ARCH_MIPS_MSA")
set(SIMDPP_MIPS_MSA_SUFFIX "-mips_msa")
set(SIMDPP_MIPS_MSA_TEST_CODE
    "#include <msa.h>
    #include <iostream>
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            v4i32 align;
        };
        char* p = data;
        p = prevent_optimization(p);
        v16i8 v = __msa_ld_b(p, 0);
        v = __msa_add_a_b(v, v);
        __msa_st_b(v, p, 0);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "POWER_ALTIVEC")
set(SIMDPP_POWER_ALTIVEC_CXX_FLAGS "-maltivec")
set(SIMDPP_POWER_ALTIVEC_DEFINE "SIMDPP_ARCH_POWER_ALTIVEC")
set(SIMDPP_POWER_ALTIVEC_SUFFIX "-power_altivec")
set(SIMDPP_POWER_ALTIVEC_TEST_CODE
    "#include <altivec.h>
    #include <iostream>
    #if defined(__GNUC__) && (__GNUC__ < 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #if !defined(__INTEL_COMPILER) && !defined(__clang__)
    #error GCC 5.0 and older are not supported on PPC little-endian. See simdpp/detail/workarounds.h
    #endif
    #endif
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            vector unsigned char align;
        };
        char* p = data;
        p = prevent_optimization(p);
        vector unsigned char v = vec_ld(0, (unsigned char*)p);
        v = vec_add(v, v);
        vec_st(v, 0, (unsigned char*)p);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "POWER_VSX_206")
set(SIMDPP_POWER_VSX_206_CXX_FLAGS "-mvsx")
set(SIMDPP_POWER_VSX_206_DEFINE "SIMDPP_ARCH_POWER_VSX_206")
set(SIMDPP_POWER_VSX_206_SUFFIX "-power_vsx_2.06")
set(SIMDPP_POWER_VSX_206_TEST_CODE
    "#include <altivec.h>
    #include <iostream>
    #if defined(__GNUC__) && (__GNUC__ < 6) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #if !defined(__INTEL_COMPILER) && !defined(__clang__)
    #error GCC 5.0 and older are not supported on PPC little-endian. See simdpp/detail/workarounds.h
    #endif
    #endif
    #if defined(__GNUC__) && (__GNUC__ < 6) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #if !defined(__INTEL_COMPILER) && !defined(__clang__)
    // Internal compiler errors or wrong behaviour on various SIMD memory operations
    #error GCC 5.x and older not supported on VSX big-endian. See simdpp/detail/workarounds.h
    #endif
    #endif
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            vector unsigned char align;
        };
        char* p = data;
        p = prevent_optimization(p);
        vector unsigned char v = vec_vsx_ld(0, (unsigned char*)p);
        v = vec_add(v, v);
        vec_vsx_st(v, 0, (unsigned char*)p);
        p = prevent_optimization(p);
    }"
)

list(APPEND SIMDPP_ARCHS_PRI "POWER_VSX_207")
set(SIMDPP_POWER_VSX_207_CXX_FLAGS "-mvsx -mcpu=power8")
set(SIMDPP_POWER_VSX_207_DEFINE "SIMDPP_ARCH_POWER_VSX_207")
set(SIMDPP_POWER_VSX_207_SUFFIX "-power_vsx_2.07")
set(SIMDPP_POWER_VSX_207_TEST_CODE
    "#include <altivec.h>
    #include <iostream>
    #if defined(__GNUC__) && (__GNUC__ < 6) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #if !defined(__INTEL_COMPILER) && !defined(__clang__)
    #error GCC 5.0 and older are not supported on PPC little-endian. See simdpp/detail/workarounds.h
    #endif
    #endif
    #if defined(__GNUC__) && (__GNUC__ < 6) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #if !defined(__INTEL_COMPILER) && !defined(__clang__)
    #error GCC 5.x and older not supported on VSX big-endian. See simdpp/detail/workarounds.h
    #endif
    #endif
    char* prevent_optimization(char* ptr)
    {
        volatile bool never = false;
        if (never) {
            while (*ptr++)
                std::cout << *ptr;
        }
        char* volatile* volatile opaque;
        opaque = &ptr;
        return *opaque;
    }
    int main()
    {
        union {
            char data[16];
            vector unsigned char align;
        };
        char* p = data;
        p = prevent_optimization(p);
        vector unsigned char v = vec_vsx_ld(0, (unsigned char*)p);
        v = vec_vpopcnt(v);
        vec_vsx_st(v, 0, (unsigned char*)p);
        p = prevent_optimization(p);
    }"
)

set(SIMDPP_ARCHS "${SIMDPP_ARCHS_PRI};${SIMDPP_ARCHS_SEC}")

# ------------------------------------------------------------------------------
# Given one arch, returns compilation flags and an unique identifier (internal)
# If the given architecture does not exist, sets both result variables to ""
#
# Arguments:
#
# - CXX_FLAGS_VAR: the name of a variable to store the compilation flags to
#
# - DEFINES_LIST_VAR: the name of a variable to store comma defimited list of
# preprocessor defines for the current architecture.
#
# - UNIQUE_ID_VAR: the name of a variable to store the unique identifier to
#
# - ARCH: an architecture
#
function(simdpp_get_arch_info CXX_FLAGS_VAR DEFINES_LIST_VAR UNIQUE_ID_VAR ARCH)
    set(UNIQUE_ID "")
    set(CXX_FLAGS "")
    set(DISPATCH_FLAGS "")
    set(DEFINES_LIST "")

    string(REPLACE "," ";" ARCH_IDS "${ARCH}")
    list(SORT ARCH_IDS)
    foreach(ID ${ARCH_IDS})
        if(${ID} STREQUAL "NONE_NULL")
            set(UNIQUE_ID "${UNIQUE_ID}-null")
        else()
            list(FIND SIMDPP_ARCHS "${ID}" FOUND)
            if(NOT ${FOUND} EQUAL -1)
                list(APPEND DEFINES_LIST "${SIMDPP_${ID}_DEFINE}")
                set(CXX_FLAGS "${CXX_FLAGS} ${SIMDPP_${ID}_CXX_FLAGS} -D${SIMDPP_${ID}_DEFINE}")
                set(UNIQUE_ID "${UNIQUE_ID}${SIMDPP_${ID}_SUFFIX}")
            endif()
        endif()
    endforeach()

    string(REPLACE ";" "," DEFINES_LIST "${DEFINES_LIST}")

    set(${CXX_FLAGS_VAR} "${CXX_FLAGS}" PARENT_SCOPE)
    set(${UNIQUE_ID_VAR} "${UNIQUE_ID}" PARENT_SCOPE)
    set(${DEFINES_LIST_VAR} "${DEFINES_LIST}" PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------------------------
#
# simdpp_multiarch(FILE_LIST_VAR SRC_FILE [ARCH...])
#
# A function that encapsulates the generation of build rules for libsimdpp
# multi-architecture source files. The function creates a copy of @a SRC_FILE
# for each supplied architecture definition. Each of these files is configured
# with appropriate compile flags for the given architecture. The list of copied
# files is appended to the variable supplied by @a FILE_LIST_VAR which can then
# be used in add_library or add_executable calls.
#
# All copied files are placed in the build directory. The directory of
# @a SRC_FILE is added to the default include paths.
#
# Arguments:
#
# * FILE_LIST_VAR: the name of the variable to append the list of generated
#   files to
#
# * SRC_FILE: the name of the source file relative to the @a
#   CMAKE_CURRENT_SOURCE_DIR
#
# * ARCH...: a list of architecture definitions. Each architecture definition
#   consist of comma separated list of identifiers directly corresponding to
#   macros defined in simdpp/simd.h, which ultimately identify instruction set
#   features. The user of the function must ensure that sensible combination of
#   identifiers is supplied.
#
#   The following identifiers are currently supported:
#   X86_SSE2, X86_SSE3, X86_SSSE3, X86_SSE4_1,
#   X86_AVX, X86_AVX2, X86_FMA3, X86_FMA4,
#   X86_AVX512F, X86_AVX512BW, X86_AVX512DQ, X86_AVX512VL, X86_XOP,
#   ARM_NEON, ARM_NEON_FLT_SP, ARM64_NEON,
#   MIPS_MSA, POWER_ALTIVEC, POWER_VSX_206, POWER_VSX_207
#
function(simdpp_multiarch FILE_LIST_VAR SRC_FILE)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_FILE}")
        message(FATAL_ERROR "File \"${SRC_FILE}\" does not exist")
    endif()
    get_filename_component(SRC_PATH "${SRC_FILE}" PATH)
    get_filename_component(SRC_NAME "${SRC_FILE}" NAME_WE)
    get_filename_component(SRC_EXT "${SRC_FILE}" EXT)

    set(FILE_LIST "")
    set(DISPATCHER_FILE "")
    set(DISPATCHER_CXX_FLAGS "-DSIMDPP_EMIT_DISPATCHER=1")
    set(DISPATCH_ARCH_IDX "1")

    list(APPEND ARCHS ${ARGV})
    list(REMOVE_AT ARCHS 0 1) # strip FILE_LIST_VAR and SRC_FILE args
    foreach(ARCH ${ARCHS})
        simdpp_get_arch_info(CXX_FLAGS DEFINES_LIST SUFFIX ${ARCH})

        set(CXX_FLAGS "-I\"${CMAKE_CURRENT_SOURCE_DIR}/${SRC_PATH}\" ${CXX_FLAGS}")
        if(NOT "${SUFFIX}" STREQUAL "")
            # Copy the source file and add the required flags
            set(DST_ABS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SRC_PATH}/${SRC_NAME}_simdpp_${SUFFIX}${SRC_EXT}")
            set(SRC_ABS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_FILE}")

            # CMake does not support adding per-source-file include directories.
            # Also when CXX_FLAGS is used for this purpose, CMake does not add
            # local includes as the dependencies of the result object file thus
            # does not rebuild the file when these included files are changed.
            # The work around is to use add_custom_command with IMPLICIT_DEPENDS
            # option which only works on make-based systems
            add_custom_command(OUTPUT "${DST_ABS_FILE}"
                               COMMAND ${CMAKE_COMMAND} -E copy "${SRC_ABS_FILE}" "${DST_ABS_FILE}"
                               IMPLICIT_DEPENDS CXX "${SRC_ABS_FILE}")

            list(APPEND FILE_LIST "${DST_ABS_FILE}")
            set_source_files_properties("${DST_ABS_FILE}" PROPERTIES COMPILE_FLAGS ${CXX_FLAGS}
                                                                     GENERATED TRUE)

            # For the first file that is being processed, set it to emit
            # dispatcher code. The required flags will be added later
            if("${DISPATCHER_FILE}" STREQUAL "")
                set(DISPATCHER_FILE "${DST_ABS_FILE}")
            endif()

            # Add required dispatcher predefined macros for this architecture
            set(DISPATCHER_CXX_FLAGS "${DISPATCHER_CXX_FLAGS} -DSIMDPP_DISPATCH_ARCH${DISPATCH_ARCH_IDX}=${DEFINES_LIST}")
            math(EXPR DISPATCH_ARCH_IDX "${DISPATCH_ARCH_IDX}+1")
        endif()
    endforeach()

    # Emit dispatcher code in the first valid generated file.
    if(NOT "${DISPATCHER_FILE}" STREQUAL "")
        set_property(SOURCE "${DISPATCHER_FILE}" APPEND_STRING PROPERTY COMPILE_FLAGS
                     "${DISPATCHER_CXX_FLAGS}")
        set(DISPATCHER_FILE "${DST_ABS_FILE}")
    endif()

    set(RECV_FILE_LIST ${${FILE_LIST_VAR}})
    list(APPEND RECV_FILE_LIST ${FILE_LIST})
    set(${FILE_LIST_VAR} ${RECV_FILE_LIST} PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------------------------
# Given a list of archs, return all possible permutations of them (internal)
#
# Arguments:
#
# - ALL_ARCHS_VAL: the name of the variable to store the permutation to
#
# - ARCH...: a list of supported architectures
function(simdpp_get_arch_perm ALL_ARCHS_VAR)
    list(APPEND ARCHS ${ARGV})
    list(REMOVE_AT ARCHS 0)

    foreach(ARCH ${ARCHS})
        set(ARCH_SUPPORTED_${ARCH} "1")
    endforeach()

    set(ALL_ARCHS "NONE_NULL")
    if(DEFINED ARCH_SUPPORTED_X86_SSE2)
        # all x86_64 processors
        list(APPEND ALL_ARCHS "X86_SSE2")
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_SSE3)
        # Since Prescott, Merom (Core architecture)
        # Since later K8 steppings, fully supported since K10
        list(APPEND ALL_ARCHS "X86_SSE3")
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_SSSE3)
        # Since Merom (Core architecture)
        # Since Bobcat and Bulldozer
        list(APPEND ALL_ARCHS "X86_SSSE3")
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_SSE4_1)
        # Since Penryl (45 nm Merom shrink)
        # Since Bulldozer
        list(APPEND ALL_ARCHS "X86_SSE4_1")
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_POPCNT_INSN)
        # Since Nehalem and K10.

        # NOTE: These two architectures are the only that support popcnt and
        # don't support AVX. There's no full overlap of the instruction set
        # support in these architectures, thus these two separate configs were
        # omitted from the default instruction set matrix.
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_AVX)
        # Since Sandy Bridge, Bulldozer, Jaguar
        list(APPEND ALL_ARCHS "X86_AVX,X86_POPCNT_INSN")

        if(DEFINED ARCH_SUPPORTED_X86_FMA3)
            # Since Haswell, Piledriver (later Bulldozer variant)
            # All CPUs in this range support popcnt
        endif()
        if(DEFINED ARCH_SUPPORTED_X86_FMA4)
            # Since Bulldozer until Zen. Jaguar does not support FMA4 nor FMA3
            # All CPUs in this range support popcnt
            list(APPEND ALL_ARCHS "X86_AVX,X86_FMA4,X86_POPCNT_INSN")
        endif()
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_AVX2)
        # Since Haswell and Zen
        # All Intel and AMD CPUs that support AVX2 also support FMA3 and POPCNT,
        # thus separate X86_AVX2 config is not needed.
        if(DEFINED ARCH_SUPPORTED_X86_FMA3)
            list(APPEND ALL_ARCHS "X86_AVX2,X86_FMA3,X86_POPCNT_INSN")
        endif()
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_FMA3)
        # Since Haswell, Piledriver (later Bulldozer variant)
        # All Intel and AMD CPUs that support FMA3 also support AVX, thus
        # separate X86_FMA3 config is not needed
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_FMA4)
        # Since Bulldozer until Zen
        # All AMD CPUs that support FMA4 also support AVX, thus
        # separate X86_FMA4 config is not needed
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_AVX512F)
        # Since Knights Landing, Skylake-X
        # All Intel CPUs that support AVX512F also support FMA3 and POPCNT,
        # thus separate X86_512F config is not needed.
        list(APPEND ALL_ARCHS "X86_AVX512F,X86_FMA3,X86_POPCNT_INSN")

        if(DEFINED ARCH_SUPPORTED_X86_AVX512BW)
            if(DEFINED ARCH_SUPPORTED_X86_AVX512DQ)
                if(DEFINED ARCH_SUPPORTED_X86_AVX512VL)
                    # All Intel processors that support AVX512BW also support
                    # AVX512DQ and AVX512VL
                    list(APPEND ALL_ARCHS "X86_AVX512F,X86_FMA3,X86_POPCNT_INSN,X86_AVX512BW,X86_AVX512DQ,X86_AVX512VL")
                endif()
            endif()
        endif()
    endif()
    if(DEFINED ARCH_SUPPORTED_X86_XOP)
        list(APPEND ALL_ARCHS "X86_XOP")
        if(DEFINED ARCH_SUPPORTED_X86_AVX)
            list(APPEND ALL_ARCHS "X86_AVX,X86_XOP")
        endif()
    endif()
    if(DEFINED ARCH_SUPPORTED_ARM_NEON)
        list(APPEND ALL_ARCHS "ARM_NEON")
        list(APPEND ALL_ARCHS "ARM_NEON_FLT_SP")
    endif()
    if(DEFINED ARCH_SUPPORTED_ARM64_NEON)
        list(APPEND ALL_ARCHS "ARM64_NEON")
    endif()
    if(DEFINED ARCH_SUPPORTED_MIPS_MSA)
        list(APPEND ALL_ARCHS "MIPS_MSA")
    endif()
    if(DEFINED ARCH_SUPPORTED_POWER_ALTIVEC)
        list(APPEND ALL_ARCHS "POWER_ALTIVEC")
    endif()
    if(DEFINED ARCH_SUPPORTED_POWER_VSX_206)
        list(APPEND ALL_ARCHS "POWER_VSX_206")
    endif()
    if(DEFINED ARCH_SUPPORTED_POWER_VSX_207)
        list(APPEND ALL_ARCHS "POWER_VSX_207")
    endif()
    set(${ALL_ARCHS_VAR} "${ALL_ARCHS}" PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------------------------
#
# simdpp_get_compilable_archs(ARCH_LIST_VAR)
#
# Returns a list of architectures that are supported by the current build
# system. The generated list may be used as an argument to simdpp_multiarch.
#
# Arguments:
#
# * ARCH_LIST_VAR: the name of the variable to put the architecture list to
#
function(simdpp_get_compilable_archs ARCH_LIST_VAR)

    foreach(ARCH ${SIMDPP_ARCHS_PRI})
        set(CMAKE_REQUIRED_FLAGS "${SIMDPP_${ARCH}_CXX_FLAGS}")
        check_cxx_source_compiles("${SIMDPP_${ARCH}_TEST_CODE}" CAN_COMPILE_${ARCH})
        if(CAN_COMPILE_${ARCH})
            list(APPEND ARCHS ${ARCH})
        endif()
    endforeach()

    simdpp_get_arch_perm(ALL_ARCHS "${ARCHS}")
    set(${ARCH_LIST_VAR} "${ALL_ARCHS}" PARENT_SCOPE)

endfunction()

# ------------------------------------------------------------------------------
#
# simdpp_get_runnable_archs(ARCH_LIST_VAR)
#
# Returns a list of architectures that are supported by the current build
# system and the processor. The generated list may be used as an argument to
# simdpp_multiarch.
#
# Arguments:
#
# * ARCH_LIST_VAR: the name of the variable to put the architecture list to
#
function(simdpp_get_runnable_archs ARCH_LIST_VAR)

    foreach(ARCH ${SIMDPP_ARCHS_PRI})
        set(CMAKE_REQUIRED_FLAGS "${SIMDPP_${ARCH}_CXX_FLAGS}")
        check_cxx_source_runs("${SIMDPP_${ARCH}_TEST_CODE}" CAN_RUN_${ARCH})
        if(CAN_RUN_${ARCH})
            list(APPEND ARCHS ${ARCH})
        endif()
    endforeach()

    simdpp_get_arch_perm(ALL_ARCHS "${ARCHS}")
    set(${ARCH_LIST_VAR} "${ALL_ARCHS}" PARENT_SCOPE)

endfunction()
