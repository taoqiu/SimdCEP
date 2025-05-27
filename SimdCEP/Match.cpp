#include "fileio.h"
#include "Match.h"
#include <algorithm>
#include <cmath>
#include <immintrin.h>
#include <chrono>
#include <unordered_map>
#include <vector>

// Remove 'using namespace std' to avoid namespace pollution
// Forward declarations for helper functions
__m512i simd_eq1(__m512i x, __m512i y, __m512i tw);
__m512i simd_eq2(__m512i y, int tw_prime);
__m512i simd_eq3(__m512i y, __m512i z, __m512i tw);
__m512i compute_shifted_mask(__m512i w_prime, int tw_prime);
void parsingResult(__m512i simdTend, int load_idx, struct global& g);
void FindPosition(struct global& g, const unsigned& k, size_t num);
void multiwordmatch(struct global& g, int i, int j, struct query& a, std::vector<std::vector<size_t>>& bitSequence,
    __m512i Tw, std::unordered_map<std::string, int>& eventmap);

// Main matching functions
void match::newbitMatch(struct global& g, struct query a, std::vector<std::vector<size_t>>& bitSequence,
    std::unordered_map<std::string, int>& eventmap, int timeContainer) {
    auto match_start = std::chrono::high_resolution_clock::now();
    int twsize = a.timeWindow / timeContainer;
    const int SIZE = 8;
    // Linux alternative for GetTickCount64()
    auto t1 = std::chrono::high_resolution_clock::now();
    int eventnum = eventmap[a.pt.evtype[a.pt.evtype.size() - 1]] - 1;

    // Padding to align data
    for (int i = (a.pt.evtype.size() - 1); i >= 0; i--) {
        int arryflag1 = eventmap[a.pt.evtype[i]] - 1;
        int fill_num = SIZE - (bitSequence[arryflag1].size() % SIZE);
        for (int t_fill_num = fill_num; t_fill_num > 0; t_fill_num--) {
            bitSequence[arryflag1].push_back(0);
        }
    }

    // Main processing loop
    int j = 0;
    int arryflag1 = eventmap[a.pt.evtype[a.pt.evtype.size() - 1]] - 1;
    __m512i signmulti = _mm512_set1_epi64(0x8000000000000000);

    while (j < bitSequence[0].size()) {
        int multiflag = 1;
        __m512i simdTend = _mm512_loadu_si512(&bitSequence[arryflag1][j]);
        __m512i simdpreTw = compute_shifted_mask(simdTend, twsize);
        __m512i simdTw = simd_eq2(simdTend, twsize);

        for (int i = (a.pt.evtype.size() - 2); i >= 0; i--) {
            int arryflag2 = eventmap[a.pt.evtype[i]] - 1;

            if (a.pt.sttype[i] == "negation") {
                __m512i simdTneg = _mm512_loadu_si512(&bitSequence[arryflag2][j]);
                simdTw = simd_eq3(simdTneg, simdTend, simdTw);
                simdTend = simd_eq1(_mm512_loadu_si512(&bitSequence[eventmap[a.pt.evtype[i - 1]] - 1][j]),
                    simdTend, simdTw);
                i--;
            }
            else {
                simdTend = simd_eq1(_mm512_loadu_si512(&bitSequence[arryflag2][j]), simdTend, simdTw);
            }

            if (!_mm512_test_epi64_mask(simdTend, simdTend)) break;

            if (_mm512_test_epi64_mask(simdTend, signmulti) && multiflag) {
                multiwordmatch(g, i, j, a, bitSequence, simdpreTw, eventmap);
                multiflag--;
            }
        }
        parsingResult(simdTend, j, g);
        j += SIZE;
    }
}

void multiwordmatch(struct global& g, int i, int j, struct query& a, std::vector<std::vector<size_t>>& bitSequence,
    __m512i Tw, std::unordered_map<std::string, int>& eventmap) {
    __m512i simdTw = Tw;
    __m512i simdTend = _mm512_set1_epi64(0x0000000000000001);
    __mmask8 mask = (j == 0) ? 0x7F : 0xFF;
    size_t load_idx = (j == 0) ? 0 : j - 1;

    if (j == 0) {
        __m512i zero = _mm512_setzero_si512();
        simdTw = _mm512_alignr_epi64(Tw, zero, 1);
    }

    for (; i >= 0; i--) {
        int arryflag2 = eventmap[a.pt.evtype[i]] - 1;

        if (a.pt.sttype[i] == "negation") {
            __m512i simdTneg = _mm512_maskz_loadu_epi64(mask, &bitSequence[arryflag2][load_idx]);
            simdTw = simd_eq3(simdTneg, simdTend, simdTw);
            simdTend = simd_eq1(_mm512_maskz_loadu_epi64(mask, &bitSequence[eventmap[a.pt.evtype[i - 1]] - 1][load_idx]),
                simdTend, simdTw);
            i--;
        }
        else {
            simdTend = simd_eq1(_mm512_maskz_loadu_epi64(mask, &bitSequence[arryflag2][load_idx]),
                simdTend, simdTw);
        }

        if (!_mm512_test_epi64_mask(simdTend, simdTend)) break;
    }
    parsingResult(simdTend, load_idx, g);
}

// SIMD helper functions
__m512i simd_eq1(__m512i x, __m512i y, __m512i tw) {
    __m512i t0 = _mm512_slli_epi64(y, 1);
    __m512i t1 = _mm512_sub_epi64(_mm512_or_epi64(x, y), t0);
    __m512i t2 = _mm512_xor_epi64(_mm512_set1_epi64(-1), t1);
    return _mm512_and_epi64(_mm512_and_epi64(x, t2), tw);
}

__m512i simd_eq2(__m512i y, int tw_prime) {
    __m512i t = y;
    for (int i = 1; i < tw_prime; i++) {
        t = _mm512_or_epi64(_mm512_slli_epi64(y, i), t);
    }
    return t;
}

__m512i simd_eq3(__m512i y, __m512i z, __m512i tw) {
    __m512i m00 = _mm512_slli_epi64(y, 1);
    __m512i m01 = _mm512_or_epi64(y, z);
    __m512i m02 = _mm512_sub_epi64(m01, m00);
    __m512i m0 = _mm512_xor_epi64(m02, _mm512_set1_epi64(-1));
    __m512i m1 = _mm512_sub_epi64(_mm512_and_epi64(m0, z), y);
    return _mm512_and_epi64(_mm512_or_epi64(_mm512_xor_epi64(m1, y), m1), tw);
}

// Other helper functions...
__m512i compute_shifted_mask(__m512i w_prime, int tw_prime) {
    __m512i msb_mask = _mm512_set1_epi64(0x7FFFFFFFFFFFFFFF);
    __m512i w_prime_cleared = _mm512_and_si512(w_prime, msb_mask);
    __m512i tw1 = _mm512_lzcnt_epi64(w_prime_cleared);
    __m512i tw_prime_vec = _mm512_set1_epi64(tw_prime);
    __m512i tw2 = _mm512_sub_epi64(tw_prime_vec, _mm512_min_epu64(tw1, tw_prime_vec));
    return _mm512_sub_epi64(_mm512_sllv_epi64(_mm512_set1_epi64(1),
        _mm512_add_epi64(tw2, _mm512_set1_epi64(1))),
        _mm512_set1_epi64(1));
}

void FindPosition(struct global& g, const unsigned& k, size_t num)
{
    size_t r1 = 0;
    size_t temp = 0;
    while (num)
    {
        static const int MultiplyDeBruijinBitPosition1[64] =
        {
            0, // change to 1 if you want bitSize(0) = 1
            1,  2, 53,  3,  7, 54, 27, 4, 38, 41,  8, 34, 55, 48, 28,
            62,  5, 39, 46, 44, 42, 22,  9, 24, 35, 59, 56, 49, 18, 29, 11,
            63, 52,  6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
            51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12
        };
        temp = (num & (-num));
        r1 = MultiplyDeBruijinBitPosition1[((temp) * 0x022FDD63CC95386DU) >> 58];
        r1 = k * 64 + (63 - r1);
        g.numresult.push_back(r1);
        g.candis.push_back(g.hmap[r1]);
        num = num - temp;
    }
}

void parsingResult(__m512i simdTend, int load_idx, struct global& g)
{
    std::vector<size_t> oneResult;
    oneResult.resize(8, 0);
    _mm512_storeu_si512(oneResult.data(), simdTend);
    for (int i = 0; i < oneResult.size(); i++)
    {
        oneResult[i] &= 0x7FFFFFFFFFFFFFFe;
        FindPosition(g, load_idx + i, oneResult[i]);
    }
}
