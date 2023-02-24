#include "big_integer.hpp"

#include <string.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int64_t Abs(const int64_t& num) {
  if (num < 0) {
    int64_t res = num * (-1);
    return res;
  }
  return num;
}

BigInt::BigInt() { array_.resize(1); }

BigInt::BigInt(const int64_t& num) {
  int num_of_digits = 0;
  int64_t num_copy = num;
  if (num < 0) {
    sign_ = false;
  }
  while (num_copy != 0) {
    num_copy /= kMaxNumInDigit + 1;
    ++num_of_digits;
  }
  if (num == 0) {
    ++num_of_digits;
  }
  num_copy = num;
  array_.resize(num_of_digits);
  for (int t_num = 0; t_num < num_of_digits; ++t_num) {
    array_[t_num] = num_copy % (kMaxNumInDigit + 1);
    if (!sign_) {
      array_[t_num] *= -1;
    }
    num_copy /= kMaxNumInDigit + 1;
  }
}

BigInt::BigInt(const std::string& num) {
  int real_size = num.size();
  if (num[0] == '-' || num[0] == '+') {
    --real_size;
  }
  int num_of_digits = real_size / kMaxNumOfDigits;
  if (real_size % kMaxNumOfDigits != 0) {
    ++num_of_digits;
  }
  array_.resize(num_of_digits);
  num[0] == '-' ? sign_ = false : sign_ = true;
  int i_num = num.size() - 1;
  for (size_t t_num = 0; t_num < array_.size(); ++t_num) {
    int64_t multiplier_for_digit = 1;
    int intermidiate_num = 0;
    for (int j = 0; j < kMaxNumOfDigits; ++j) {
      intermidiate_num += (num[i_num] - '0') * multiplier_for_digit;
      const int kDigitInNormalNumber = 10;
      multiplier_for_digit *= kDigitInNormalNumber;
      --i_num;
      if (i_num == -1 || num[i_num] == '-' || num[i_num] == '+') {
        break;
      }
    }
    array_[t_num] = intermidiate_num;
  }
  if (array_[0] == 0) {
    sign_ = true;
  }
}

BigInt& BigInt::operator=(const BigInt& num) = default;

BigInt::BigInt(const BigInt& num) = default;

BigInt::~BigInt() {}

bool operator<(const BigInt& first, const BigInt& second) {
  if (first.sign_ != second.sign_) {
    return static_cast<int>(first.sign_) < static_cast<int>(second.sign_);
  }
  if (first.array_.size() != second.array_.size()) {
    return first.array_.size() < second.array_.size();
  }
  for (size_t i = first.array_.size(); i > 0; --i) {
    if (first.array_[i - 1] != second.array_[i - 1]) {
      if (!first.sign_) {
        return !(first.array_[i - 1] < second.array_[i - 1]);
      }
      return first.array_[i - 1] < second.array_[i - 1];
    }
  }
  return false;
}

BigInt BigInt::operator-() const {
  BigInt res = *this;
  if (res == 0) {
    return res;
  }
  res.sign_ = !sign_;
  return res;
}

BigInt& BigInt::operator++() { return *this += 1; }

BigInt BigInt::operator++(int) {
  BigInt res = *this;
  *this += 1;
  return res;
}

BigInt& BigInt::operator--() { return *this -= 1; }

BigInt BigInt::operator--(int) {
  BigInt res = *this;
  *this -= 1;
  return res;
}

BigInt& BigInt::operator+=(const BigInt& num) {
  if (sign_ == num.sign_) {
    array_.resize(std::max(array_.size(), num.array_.size()));
    for (size_t i = 0; i < std::max(array_.size(), num.array_.size()); ++i) {
      if (num.array_.size() > i) {
        array_[i] += num.array_[i];
      }
      if (array_[i] > kMaxNumInDigit) {
        if (i + 1 > array_.size() - 1) {
          array_.resize(array_.size() + 1);
        }
        ++array_[i + 1];
      }
      array_[i] %= kMaxNumInDigit + 1;
    }
    return *this;
  }
  if (!num.sign_) {
    sign_ = false;
    *this -= num;
    sign_ = !(sign_ && *this != 0);
    return *this;
  }
  if (!sign_) {
    sign_ = true;
    *this -= num;
    sign_ = !(sign_ && *this != 0);
    return *this;
  }
  return *this;
}

BigInt& BigInt::FirstCaseForSubstraction(const BigInt& num) {
  for (size_t i = 0; i < std::min(array_.size(), num.array_.size()); ++i) {
    array_[i] -= num.array_[i];
    if (array_[i] < 0) {
      --array_[i + 1];
      if (array_[i + 1] == 0) {
        array_.resize(array_.size() - 1);
      }
      array_[i] += kMaxNumInDigit + 1;
    }
  }
  for (size_t i = num.array_.size(); i < array_.size(); ++i) {
    if (array_[i] < 0) {
      --array_[i + 1];
      if (array_[i + 1] == 0) {
        array_.resize(array_.size() - 1);
      }
      array_[i] += kMaxNumInDigit + 1;
    }
  }
  size_t real_size = array_.size();
  for (int i = array_.size() - 1; i >= 0; --i) {
    if (array_[i] == 0) {
      --real_size;
    } else {
      break;
    }
  }
  array_.resize(std::max(real_size, static_cast<size_t>(1)));
  return *this;
}

BigInt& BigInt::SecondCaseForSubstraction(const BigInt& num) {
  sign_ = false;
  array_.resize(std::max(array_.size(), num.array_.size()));
  int minus_one = 0;
  for (size_t i = 0; i < std::min(array_.size(), num.array_.size()); ++i) {
    int64_t intermideiate = 0;
    intermideiate = num.array_[i] - array_[i] - minus_one;
    array_[i] = intermideiate;
    if (array_[i] < 0) {
      minus_one = 1;
      array_[i] += kMaxNumInDigit + 1;
    } else {
      minus_one = 0;
    }
  }
  return *this;
}

BigInt& BigInt::RealCountForSubstraction(const BigInt& num) {
  if (*this >= num) {
    return FirstCaseForSubstraction(num);
  }
  return SecondCaseForSubstraction(num);
}

BigInt& BigInt::operator-=(const BigInt& num) {
  BigInt copy = num;
  if (sign_ == num.sign_) {
    if (!sign_) {
      copy.sign_ = true;
      return *this += copy;
    }
    return RealCountForSubstraction(num);
  }
  if (!sign_) {
    copy.sign_ = false;
    return *this += copy;
  }
  copy.sign_ = true;
  return *this += copy;
}

BigInt& BigInt::operator*=(const BigInt& num) {
  if (*this == 0 || num == 0) {
    *this = 0;
    return *this;
  }
  std::vector<uint64_t> intermidiate;
  intermidiate.resize(array_.size() + num.array_.size());
  for (size_t i = 0; i < num.array_.size(); ++i) {
    for (size_t j = 0; j < array_.size(); ++j) {
      intermidiate[i + j] += array_[j] * num.array_[i];
    }
  }
  for (size_t i = 0; i < intermidiate.size(); ++i) {
    if (intermidiate[i] > kMaxNumInDigit) {
      intermidiate[i + 1] += intermidiate[i] / (kMaxNumInDigit + 1);
      intermidiate[i] %= kMaxNumInDigit + 1;
    }
  }
  if (intermidiate[intermidiate.size() - 1] == 0) {
    intermidiate.resize(intermidiate.size() - 1);
  }
  array_.resize(intermidiate.size());
  for (size_t i = 0; i < intermidiate.size(); ++i) {
    array_[i] = intermidiate[i];
  }
  sign_ == num.sign_ ? sign_ = true : sign_ = false;
  return *this;
}

BigInt* BigInt::CasesForDevision(const BigInt& num) {
  sign_ == num.sign_ ? sign_ = true : sign_ = false;
  if (*this == num) {
    *this = 1;
    return this;
  }
  if (num > *this && sign_) {
    *this = 0;
    return this;
  }
  if (num < *this && !(sign_)) {
    *this = 0;
    return this;
  }
  if (num == 1) {
    return this;
  }
  return nullptr;
}

void BigInt::HelperFunctionForDevision(int& size, BigInt& inter) {
  --size;
  inter *= kMaxNumInDigit + 1;
  inter += array_[size];
}

BigInt& BigInt::operator/=(const BigInt& num) {
  BigInt* interm = CasesForDevision(num);
  if (interm != nullptr) {
    return *interm;
  }
  BigInt inter;
  int size = array_.size();
  std::vector<int64_t> intermidiate;
  while (inter * (kMaxNumInDigit + 1) + array_[size - 1] < num) {
    HelperFunctionForDevision(size, inter);
  }
  while (size > 0) {
    HelperFunctionForDevision(size, inter);
    int64_t intermidiate_num = kMaxNumInDigit;
    BigInt test = intermidiate_num * num;
    while (intermidiate_num * num > inter) {
      --intermidiate_num;
    }
    inter -= intermidiate_num * num;
    if (inter.array_[inter.array_.size() - 1] == 0) {
      inter.array_.resize(inter.array_.size() - 1);
    }
    intermidiate.push_back(intermidiate_num);
  }
  array_.resize(intermidiate.size());
  for (size_t i = 0; i < array_.size(); ++i) {
    array_[i] = intermidiate[intermidiate.size() - i - 1];
  }
  return *this;
}

BigInt& BigInt::operator%=(const BigInt& num) {
  BigInt intermidiate = *this / num;
  *this -= num * intermidiate;
  return *this;
}

bool operator>(const BigInt& first, const BigInt& second) {
  return second < first;
}

bool operator<=(const BigInt& first, const BigInt& second) {
  return !(first > second);
}

bool operator>=(const BigInt& first, const BigInt& second) {
  return !(first < second);
}

bool operator==(const BigInt& first, const BigInt& second) {
  return (first >= second && first <= second);
}

bool operator!=(const BigInt& first, const BigInt& second) {
  return !(first == second);
}

BigInt operator+(const BigInt& first, const BigInt& second) {
  BigInt res = first;
  res += second;
  return res;
}

BigInt operator-(const BigInt& first, const BigInt& second) {
  BigInt res = first;
  res -= second;
  return res;
}

BigInt operator*(const BigInt& first, const BigInt& second) {
  BigInt res = first;
  res *= second;
  return res;
}

BigInt operator/(const BigInt& first, const BigInt& second) {
  BigInt res = first;
  res /= second;
  return res;
}

BigInt operator%(const BigInt& first, const BigInt& second) {
  BigInt res = first;
  res %= second;
  return res;
}

std::istream& operator>>(std::istream& is_stream, BigInt& second) {
  std::string str;
  char intermidiate = 0;
  intermidiate = is_stream.get();
  while (intermidiate != ' ' && intermidiate != '\n' && intermidiate != EOF) {
    str.push_back(intermidiate);
    intermidiate = is_stream.get();
  }
  second = str;
  return is_stream;
}

std::ostream& operator<<(std::ostream& os_stream, const BigInt& second) {
  std::string str;
  std::setfill('0');
  std::setw(BigInt::kMaxNumInDigit);
  str.reserve(second.array_.size() * BigInt::kMaxNumInDigit);
  if (!second.sign_) {
    os_stream << '-';
  }
  os_stream << second.array_[second.array_.size() - 1];
  int64_t size = second.array_.size();
  for (int i = size - 2; i >= 0; --i) {
    os_stream << std::setfill('0') << std::setw(BigInt::kMaxNumOfDigits)
              << second.array_[i];
  }
  return os_stream;
}