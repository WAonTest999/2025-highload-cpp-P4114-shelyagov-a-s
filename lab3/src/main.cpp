#include <atomic>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <functional>
#include <future>
#include <immintrin.h>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

class Spin_lock {
  std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
  void lock() noexcept {
    while (flag.test_and_set(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }
  void unlock() noexcept { flag.clear(std::memory_order_release); }
};

class Thread_pool {
private:
  Spin_lock queue_lock;
  std::queue<std::function<void()>> tasks;
  std::atomic<bool> stop;
  std::atomic<std::size_t> tasks_count;
  std::vector<std::thread> workers;

  void worker_loop() {
    while (!(stop.load(std::memory_order_acquire) &&
             tasks_count.load(std::memory_order_acquire) == 0)) {
      if (tasks_count.load(std::memory_order_acquire) > 0) {
        std::unique_lock<Spin_lock> lock(queue_lock);
        if (!tasks.empty()) {
          auto task = std::move(tasks.front());
          tasks.pop();
          tasks_count.fetch_sub(1, std::memory_order_release);
          lock.unlock();
          task();
          continue;
        }
      }
      std::this_thread::yield();
    }
  }

public:
  Thread_pool(std::size_t thread_count = std::thread::hardware_concurrency())
      : stop(false), tasks_count(0) {
    workers.reserve(thread_count);
    for (std::size_t i = 0; i < thread_count; ++i) {
      workers.emplace_back([this] { worker_loop(); });
    }
  }

  ~Thread_pool() {
    stop.store(true, std::memory_order_release);
    for (auto &t : workers) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  template <typename T, typename Fn> std::future<T> dispatch_task(Fn &&f) {
    auto task = std::make_shared<std::packaged_task<T()>>(std::forward<Fn>(f));
    std::future<T> result = task->get_future();

    {
      std::unique_lock<Spin_lock> lock(queue_lock);
      tasks.emplace([task]() { (*task)(); });
    }
    tasks_count.fetch_add(1, std::memory_order_release);
    return result;
  }
};

size_t test_solution_thread(float *arr_in, float *arr_out, size_t size,
                            float val, Thread_pool &pool,
                            size_t blockSize = 1024) {
  if (size == 0) {
    return 0;
  }

  size_t blocks_count = (size + blockSize - 1) / blockSize;
  std::vector<size_t> block_indexes(blocks_count, 0);
  std::vector<size_t> block_starts(blocks_count + 1, 0);

  std::vector<std::future<void>> count_futures;
  count_futures.reserve(blocks_count);
  for (size_t b = 0; b < blocks_count; ++b) {
    size_t start = b * blockSize;
    size_t end = std::min(start + blockSize, size);
    count_futures.emplace_back(pool.dispatch_task<void>([=, &block_indexes]() {
      size_t cnt = 0;
      for (size_t i = start; i < end; ++i) {
        if (arr_in[i] > val)
          ++cnt;
      }
      block_indexes[b] = cnt;
    }));
  }
  for (auto &fut : count_futures)
    fut.get();

  size_t total = 0;
  for (size_t b = 0; b < blocks_count; ++b) {
    block_starts[b] = total;
    total += block_indexes[b];
  }
  block_starts[blocks_count] = total;

  std::vector<std::future<void>> write_futures;
  write_futures.reserve(blocks_count);
  for (size_t b = 0; b < blocks_count; ++b) {
    size_t start = b * blockSize;
    size_t end = std::min(start + blockSize, size);
    size_t out_pos = block_starts[b];
    write_futures.emplace_back(pool.dispatch_task<void>([=, &arr_out]() {
      size_t local_pos = out_pos;
      for (size_t i = start; i < end; ++i) {
        if (arr_in[i] > val) {
          arr_out[local_pos++] = arr_in[i];
        }
      }
    }));
  }
  for (auto &fut : write_futures)
    fut.get();

  return total;
}

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

int main() {

  const size_t test_size = 10000;
  uint64_t time_stupid_floats[test_size];
  uint64_t time_simd_floats[test_size];
  uint64_t time_thread_floats[test_size];
  uint64_t time_thread2_floats[test_size];

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

  int thread_count = std::thread::hardware_concurrency();

  for (int i = 0; i < test_size; ++i) {
    auto time_start = std::chrono::steady_clock::now();
    Thread_pool pool(thread_count);
    test_solution_thread(test_floats, test_floats, i, 0.f, pool,
                         test_size / thread_count);
    auto time_end = std::chrono::steady_clock::now();
    time_thread_floats[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();
    if (i % 100 == 0) {
      std::cout << "Test floats thread: \t" << i << '\n';
    }
  }

  for (int i = 0; i < test_size; ++i) {
    Thread_pool pool(thread_count);
    auto time_start = std::chrono::steady_clock::now();
    test_solution_thread(test_floats, test_floats, i, 0.f, pool,
                         test_size / thread_count);
    auto time_end = std::chrono::steady_clock::now();
    time_thread2_floats[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();
    if (i % 100 == 0) {
      std::cout << "Test floats thread: \t" << i << '\n';
    }
  }

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

  out.open("results/floats_thread.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_thread_floats[i] << '\n';
  }
  out.close();

  out.open("results/floats_thread2.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_thread2_floats[i] << '\n';
  }
  out.close();

  float avg_floats_stupid = 0;
  float avg_floats_simd = 0;
  float avg_floats_thread = 0;
  float avg_floats_thread2 = 0;

  for (size_t i = 1; i < test_size; ++i) {
    avg_floats_stupid += float(time_stupid_floats[i]) / i;
    avg_floats_simd += float(time_simd_floats[i]) / i;
    avg_floats_thread += float(time_thread_floats[i]) / i;
    avg_floats_thread2 += float(time_thread2_floats[i]) / i;
  }

  avg_floats_stupid /= test_size;
  avg_floats_simd /= test_size;
  avg_floats_thread /= test_size;
  avg_floats_thread2 /= test_size;

  std::cout << "\navg stupid floats: " << avg_floats_stupid << '\n';
  std::cout << "avg simd floats:   " << avg_floats_simd << '\n';
  std::cout << "avg thread floats  " << avg_floats_thread << '\n';
  std::cout << "avg thread2 floats " << avg_floats_thread2 << '\n';
  std::cout << "Threads: " << std::thread::hardware_concurrency();
}
