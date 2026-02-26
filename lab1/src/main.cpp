#include "./HashTable.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

int64_t temp;

void prepare_same_values(std::vector<uint64_t> &vec, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    vec.push_back(i * (1ULL << 32));
  }
}

void prepare_rand_values(std::vector<uint64_t> &vec, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    vec.push_back(i);
  }
}

void fill_values(HashTable<uint64_t> &table, std::vector<uint64_t> values,
                 size_t size) {
  for (size_t i = 0; i < size; ++i) {
    table.insert(values[i], 0);
  }
}

void fill_with_random(HashTable<uint64_t> &table, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    table.insert(i, 0);
  }
}

void find_same(HashTable<uint64_t> &table, std::vector<uint64_t> vec,
               int64_t size) {
  for (size_t i = 0; i < size; ++i) {
    temp = table.get(vec[i]);
  }
}

void find_random(HashTable<uint64_t> table, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    temp = table.get(i);
  }
}

void remove_values(HashTable<uint64_t> &table, std::vector<uint64_t> &vec,
                   size_t size) {
  for (size_t i = 0; i < size; ++i) {
    table.remove(vec[size - i]);
  }
}

int main() {

  std::vector<uint64_t> same_values;
  std::vector<uint64_t> rand_values;
  const size_t test_size = 10000;

  size_t time_insertion_same[test_size];
  size_t time_deletion_same[test_size];
  size_t time_find_same[test_size];
  size_t time_insertion_rand[test_size];
  size_t time_deletion_rand[test_size];
  size_t time_find_rand[test_size];

  prepare_same_values(same_values, test_size);
  prepare_rand_values(rand_values, test_size);

  for (size_t i = 0; i < test_size; ++i) {

    HashTable<uint64_t> table;
    auto time_start = std::chrono::steady_clock::now();
    fill_values(table, same_values, i);
    auto time_end = std::chrono::steady_clock::now();
    time_insertion_same[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();

    time_start = std::chrono::steady_clock::now();
    find_same(table, same_values, i);
    time_end = std::chrono::steady_clock::now();
    time_find_same[i] = (std::chrono::duration_cast<std::chrono::microseconds>(
                             time_end - time_start))
                            .count();

    time_start = std::chrono::steady_clock::now();
    remove_values(table, same_values, i);
    time_end = std::chrono::steady_clock::now();
    time_deletion_same[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();
    if (i % 100 == 0) {
      std::cout << "Test size:\t" << i << '\n';
    }
  }

  for (size_t i = 0; i < test_size; ++i) {

    HashTable<uint64_t> table;
    auto time_start = std::chrono::steady_clock::now();
    fill_values(table, rand_values, i);
    auto time_end = std::chrono::steady_clock::now();
    time_insertion_rand[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();

    time_start = std::chrono::steady_clock::now();
    find_random(table, i);
    time_end = std::chrono::steady_clock::now();
    time_find_rand[i] = (std::chrono::duration_cast<std::chrono::microseconds>(
                             time_end - time_start))
                            .count();

    time_start = std::chrono::steady_clock::now();
    remove_values(table, rand_values, i);
    time_end = std::chrono::steady_clock::now();
    time_deletion_rand[i] =
        (std::chrono::duration_cast<std::chrono::microseconds>(time_end -
                                                               time_start))
            .count();
    if (i % 100 == 0) {
      std::cout << "Test size:\t" << i << '\n';
    }
  }

  std::ofstream out;

  out.open("results/insertion_same.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_insertion_same[i] << '\n';
  }
  out.close();

  out.open("results/find_same.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_find_same[i] << '\n';
  }
  out.close();

  out.open("results/deletion_same.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_deletion_same[i] << '\n';
  }
  out.close();

  out.open("results/insertion_rand.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_insertion_rand[i] << '\n';
  }
  out.close();

  out.open("results/find_rand.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_find_rand[i] << '\n';
  }
  out.close();

  out.open("results/deletion_rand.txt");
  for (size_t i = 0; i < test_size; ++i) {
    out << time_deletion_rand[i] << '\n';
  }
  out.close();
}
