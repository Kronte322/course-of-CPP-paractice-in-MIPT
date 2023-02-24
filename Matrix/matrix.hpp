#pragma once

#include <algorithm>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  Matrix() : table_(N, std::vector<T>(M)) {}

  Matrix(const std::vector<std::vector<T>>& vector) : table_{vector} {}

  Matrix(const Matrix& other) : table_{other.table_} {};

  Matrix(const T& elem) : table_(N, std::vector<T>(M, elem)) {}

  Matrix& operator+=(const Matrix& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        table_[i][j] += other.table_[i][j];
      }
    }
    return *this;
  }

  Matrix& operator=(const Matrix& other) {
    this->table_ = other.table_;
    return *this;
  }

  Matrix& operator-=(const Matrix& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        table_[i][j] -= other(i, j);
      }
    }
    return *this;
  }

  Matrix<M, N, T> Transposed() const {
    Matrix<M, N, T> res;
    for (size_t i = 0; i < M; ++i) {
      for (size_t j = 0; j < N; ++j) {
        res(i, j) = table_[j][i];
      }
    }
    return res;
  }

  T TraceReal(Matrix<N, N, T> mat) {
    T res = T();
    for (size_t i = 0; i < table_.size(); ++i) {
      res += mat(i, i);
    }
    return res;
  }

  T Trace() { return TraceReal(*this); }

  T& operator()(const size_t& i_num, const size_t& j_num) {
    return this->table_[i_num][j_num];
  }

  const T& operator()(const size_t& i_num, const size_t& j_num) const {
    return this->table_[i_num][j_num];
  }

  bool operator==(const Matrix& second) { return table_ = second.table_; }

  template <size_t K>
  Matrix<N, K, T> operator*(const Matrix<M, K, T>& second) {
    Matrix<N, K, T> res;
    for (size_t j = 0; j < K; ++j) {
      for (size_t t_num = 0; t_num < M; ++t_num) {
        for (size_t i = 0; i < N; ++i) {
          res.GetTable()[i][j] +=
              table_[i][t_num] * second.GetTable()[t_num][j];
        }
      }
    }
    return res;
  }

  template <typename K>
  Matrix<N, M, T> operator*(const K& elem) {
    Matrix<N, M, T> res = *this;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        res.table_[i][j] *= elem;
      }
    }
    return res;
  }

  std::vector<std::vector<T>>& GetTable() { return table_; }

  const std::vector<std::vector<T>>& GetTable() const { return table_; }

 private:
  std::vector<std::vector<T>> table_;
};

template <size_t N, size_t M, typename T = int64_t>
Matrix<N, M, T> operator+(const Matrix<N, M, T>& first,
                          const Matrix<N, M, T>& second) {
  Matrix<N, M, T> res = first;
  res += second;
  return res;
}

template <size_t N, size_t M, typename T = int64_t>
Matrix<N, M, T> operator-(const Matrix<N, M, T>& first,
                          const Matrix<N, M, T>& second) {
  Matrix<N, M, T> res = first;
  res -= second;
  return res;
}