#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <immintrin.h>
#include <iostream>

void prepare_rand_floats(float *arr, size_t size) {
  for (size_t i = 0; i < size; i++) {
    arr[i] = (rand() - RAND_MAX / 2) / 1000.f;
  }
}

size_t stupid_solution_floats(float *arr_in, float *arr_out, size_t size,
                              float val) {
  size_t curr_idx_out = 0;
  for (size_t i = 0; i < size; i++) {
    if (arr_in[i] > val) {
      arr_out[curr_idx_out] = arr_in[i];
      curr_idx_out++;
    }
  }
  return curr_idx_out;
}

size_t simd_solution_floats(const float *arr_in, float *arr_out, size_t size,
                            float val) {
  size_t i = 0;
  size_t curr_idx_out = 0;
  __m256 val8 = _mm256_set1_ps(val);

  for (; i + 7 < size; i += 8) {
    __m256 packed = _mm256_loadu_ps(arr_in + i);
    __m256 res = _mm256_cmp_ps(packed, val8, _CMP_GT_OS);

    int mask = _mm256_movemask_ps(res);
    if (mask == 0) {
      continue;
    }
    if (mask == 0xFF) {
      _mm256_storeu_ps(arr_out + curr_idx_out, packed);
      curr_idx_out += 8;
    } else {
      for (size_t j = 0; j < 8; ++j) {
        if (mask & (1 << j)) {
          arr_out[curr_idx_out] = arr_in[i + j];
          curr_idx_out++;
        }
      }
    }
  }
  for (; i < size; ++i) {
    if (arr_in[i] > val) {
      arr_out[curr_idx_out] = arr_in[i];
      curr_idx_out++;
    }
  }

  return curr_idx_out;
}

void prepare_img(uint32_t *arr, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    arr[i] = rand() % 256;
    arr[i] = (arr[i] << 8) | (rand() % 256);
    arr[i] = (arr[i] << 8) | (rand() % 256);
    arr[i] = (arr[i] << 8) | (rand() % 256);
  }
}

uint64_t stupid_solution_img(const uint32_t *img1, const uint32_t *img2,
                             size_t size) {
  uint64_t sum = 0;
  for (size_t i = 0; i < size; ++i) {
    uint8_t r1 = img1[i] & 0xFF;
    uint8_t g1 = (img1[i] << 8) & 0xFF;
    uint8_t b1 = (img1[i] << 16) & 0xFF;
    uint8_t a1 = (img1[i] << 24) & 0xFF;
    uint8_t r2 = img2[i] & 0xFF;
    uint8_t g2 = (img2[i] << 8) & 0xFF;
    uint8_t b2 = (img2[i] << 16) & 0xFF;
    uint8_t a2 = (img2[i] << 24) & 0xFF;
    sum += abs(r1 - r2);
    sum += abs(g1 - g2);
    sum += abs(b1 - b2);
    sum += abs(a1 - a2);
  }
  return sum;
}

uint64_t simd_solution_img(const uint32_t *img1, const uint32_t *img2,
                           size_t size) {
  __m128i sum = _mm_setzero_si128();
  size_t i = 0;

  for (; i + 3 < size; i += 4) {
    __m128i packed1 =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(img1 + i));
    __m128i packed2 =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(img2 + i));

    __m128i sad = _mm_sad_epu8(packed1, packed2);

    sum = _mm_add_epi64(sum, sad);
  }

  uint64_t ans = _mm_cvtsi128_si64(sum) + _mm_extract_epi64(sum, 1);

  for (; i < size; ++i) {
    uint8_t r1 = img1[i] & 0xFF;
    uint8_t g1 = (img1[i] >> 8) & 0xFF;
    uint8_t b1 = (img1[i] >> 16) & 0xFF;
    uint8_t a1 = (img1[i] >> 24) & 0xFF;
    uint8_t r2 = img2[i] & 0xFF;
    uint8_t g2 = (img2[i] >> 8) & 0xFF;
    uint8_t b2 = (img2[i] >> 16) & 0xFF;
    uint8_t a2 = (img2[i] >> 24) & 0xFF;
    ans += abs(r1 - r2);
    ans += abs(g1 - g2);
    ans += abs(b1 - b2);
    ans += abs(a1 - a2);
  }

  return ans;
}

void prepare_sequence(std::string *arr, size_t size) {
  int open_count = 1;
  if (size > 0) {
    arr[0] = "(";
  }
  for (size_t i = 1; i < size; ++i) {
    if (open_count > 0 && rand() & 1) {
      arr[i] = arr[i - 1] + ")";
      open_count--;
    } else {
      arr[i] = arr[i - 1] + "(";
      open_count++;
    }
  }
}

bool stupid_solution_sequence(const std::string &str, size_t size) {
  int open_count = 0;
  for (size_t i = 0; i < size; ++i) {
    if (str[i] == '(') {
      open_count++;
    } else {
      open_count--;
    }
    if (open_count < 0) {
      return false;
    }
  }
  if (open_count != 0) {
    return false;
  }
  return true;
}

bool simd_solution_sequence(const std::string &str, size_t size) {
  const char *data = str.data();
  int open_count = 0;
  size_t i = 0;

  for (; i + 32 <= size; i += 32) {
    __m256i packed =
        _mm256_loadu_si256(reinterpret_cast<const __m256i *>(data + i));

    __m256i open = _mm256_set1_epi8('(');
    __m256i mask = _mm256_cmpeq_epi8(packed, open);

    __m256i plus1 = _mm256_set1_epi8(1);
    __m256i minus1 = _mm256_set1_epi8(-1);
    __m256i values = _mm256_blendv_epi8(plus1, minus1, mask);

    __m128i lo = _mm256_extracti128_si256(values, 0);
    __m128i hi = _mm256_extracti128_si256(values, 1);

    __m128i sum1 = _mm_add_epi8(lo, _mm_bslli_si128(lo, 1));
    __m128i sum2 = _mm_add_epi8(sum1, _mm_bslli_si128(sum1, 2));
    __m128i sum4 = _mm_add_epi8(sum2, _mm_bslli_si128(sum2, 4));
    __m128i lo_pref = _mm_add_epi8(sum4, _mm_bslli_si128(sum4, 8));
    signed char lo_total = (signed char)_mm_extract_epi8(lo_pref, 15);

    sum1 = _mm_add_epi8(hi, _mm_bslli_si128(hi, 1));
    sum2 = _mm_add_epi8(sum1, _mm_bslli_si128(sum1, 2));
    sum4 = _mm_add_epi8(sum2, _mm_bslli_si128(sum2, 4));
    __m128i hi_prefix_raw = _mm_add_epi8(sum4, _mm_bslli_si128(sum4, 8));

    __m128i add = _mm_set1_epi8(lo_total);
    __m128i hi_pref = _mm_add_epi8(hi_prefix_raw, add);

    __m128i min_lo = lo_pref;
    min_lo = _mm_min_epi8(min_lo, _mm_bsrli_si128(min_lo, 8));
    min_lo = _mm_min_epi8(min_lo, _mm_bsrli_si128(min_lo, 4));
    min_lo = _mm_min_epi8(min_lo, _mm_bsrli_si128(min_lo, 2));
    min_lo = _mm_min_epi8(min_lo, _mm_bsrli_si128(min_lo, 1));
    int min_lo_fin = (signed char)_mm_extract_epi8(min_lo, 0);

    __m128i min_hi = hi_pref;
    min_hi = _mm_min_epi8(min_hi, _mm_bsrli_si128(min_hi, 8));
    min_hi = _mm_min_epi8(min_hi, _mm_bsrli_si128(min_hi, 4));
    min_hi = _mm_min_epi8(min_hi, _mm_bsrli_si128(min_hi, 2));
    min_hi = _mm_min_epi8(min_hi, _mm_bsrli_si128(min_hi, 1));
    int min_hi_fin = (signed char)_mm_extract_epi8(min_hi, 0);

    int min_prefix = (min_lo_fin < min_hi_fin) ? min_lo_fin : min_hi_fin;

    if (open_count + min_prefix < 0) {
      return false;
    }

    signed char total = (signed char)_mm_extract_epi8(hi_pref, 15);
    open_count += total;
  }

  for (; i < size; ++i) {
    if (data[i] == '(') {
      open_count++;
    } else {
      open_count--;
    }
    if (open_count < 0)
      return false;
  }
  if (open_count != 0) {
    return false;
  }
  return true;
}

int main() {

  const size_t test_size = 10000;
  uint64_t time_stupid_floats[test_size];
  uint64_t time_simd_floats[test_size];
  uint64_t time_stupid_img[test_size];
  uint64_t time_simd_img[test_size];
  uint64_t time_stupid_sequence[test_size];
  uint64_t time_simd_sequence[test_size];

  float test_floats[test_size];
  float result[test_size];
  prepare_rand_floats(test_floats, test_size);
  for (int i = 0; i < test_size; ++i) {
    auto time_start = std::chrono::steady_clock::now();
    stupid_solution_floats(test_floats, test_floats, i, 0.f);
    auto time_end = std::chrono::steady_clock::now();
    time_stupid_floats[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();
    if (i % 100 == 0) {
      std::cout << "Test floats stupid: \t" << i << '\n';
    }
  }

  for (int i = 0; i < test_size; ++i) {
    auto time_start = std::chrono::steady_clock::now();
    simd_solution_floats(test_floats, test_floats, i, 0.f);
    auto time_end = std::chrono::steady_clock::now();
    time_simd_floats[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();
    if (i % 100 == 0) {
      std::cout << "Test floats simd: \t" << i << '\n';
    }
  }

  uint32_t test_img1[test_size], test_img2[test_size];
  prepare_img(test_img1, test_size);
  prepare_img(test_img2, test_size);

  for (int i = 0; i < test_size; ++i) {
    auto time_start = std::chrono::steady_clock::now();
    stupid_solution_img(test_img1, test_img2, i);
    auto time_end = std::chrono::steady_clock::now();
    time_stupid_img[i] = (std::chrono::duration_cast<std::chrono::microseconds>(
                              time_end - time_start))
                             .count();
    if (i % 100 == 0) {
      std::cout << "Test img stupid: \t" << i << '\n';
    }
  }

  for (int i = 0; i < test_size; ++i) {
    auto time_start = std::chrono::steady_clock::now();
    simd_solution_img(test_img1, test_img2, i);
    auto time_end = std::chrono::steady_clock::now();
    time_simd_img[i] = (std::chrono::duration_cast<std::chrono::microseconds>(
                            time_end - time_start))
                           .count();
    if (i % 100 == 0) {
      std::cout << "Test img stupid: \t" << i << '\n';
    }
  }

  std::string test_str[test_size];
  prepare_sequence(test_str, test_size);

  for (int i = 0; i < test_size; ++i) {
    auto time_start = std::chrono::steady_clock::now();
    stupid_solution_sequence(test_str[i], i);
    auto time_end = std::chrono::steady_clock::now();
    time_stupid_sequence[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();
    if (i % 100 == 0) {
      std::cout << "Test sequence stupid: \t" << i << '\n';
    }
  }

  for (int i = 0; i < test_size; ++i) {
    auto time_start = std::chrono::steady_clock::now();
    simd_solution_sequence(test_str[i], i);
    auto time_end = std::chrono::steady_clock::now();
    time_simd_sequence[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();
    if (i % 100 == 0) {
      std::cout << "Test sequence simd: \t" << i << '\n';
    }
  }
  std::cout << test_str[20];
  std::ofstream out;

  out.open("results/floats_stupid.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_stupid_floats[i] << '\n';
  }
  out.close();

  out.open("results/floats_simd.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_simd_floats[i] << '\n';
  }
  out.close();

  out.open("results/img_stupid.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_stupid_img[i] << '\n';
  }
  out.close();

  out.open("results/img_simd.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_simd_img[i] << '\n';
  }
  out.close();

  out.open("results/sequence_stupid.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_stupid_sequence[i] << '\n';
  }
  out.close();

  out.open("results/sequence_simd.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_simd_sequence[i] << '\n';
  }
  out.close();

  float avg_floats_stupid = 0;
  float avg_floats_simd = 0;
  float avg_img_stupid = 0;
  float avg_img_simd = 0;
  float avg_sequence_stupid = 0;
  float avg_sequence_simd = 0;

  for (size_t i = 1; i < test_size; ++i) {
    avg_floats_stupid += float(time_stupid_floats[i]) / i;
    avg_floats_simd += float(time_simd_floats[i]) / i;
    avg_img_stupid += float(time_stupid_img[i]) / i;
    avg_img_simd += float(time_simd_img[i]) / i;
    avg_sequence_stupid += float(time_stupid_sequence[i]) / i;
    avg_sequence_simd += float(time_simd_sequence[i]) / i;
  }

  avg_floats_stupid /= test_size;
  avg_floats_simd /= test_size;
  avg_img_stupid /= test_size;
  avg_img_simd /= test_size;
  avg_sequence_stupid /= test_size;
  avg_sequence_simd /= test_size;

  std::cout << "\navg stupid floats: " << avg_floats_stupid << '\n';
  std::cout << "avg simd floats:   " << avg_floats_simd << '\n';
  std::cout << "avg stupid img:    " << avg_img_stupid << '\n';
  std::cout << "avg simd img:      " << avg_img_simd << '\n';
  std::cout << "avg stupid seq:    " << avg_sequence_stupid << '\n';
  std::cout << "avg simd seq:      " << avg_sequence_simd << '\n';
}
