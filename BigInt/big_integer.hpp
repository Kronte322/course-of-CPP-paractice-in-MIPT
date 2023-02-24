#pragma once
#include <string.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

class BigInt {
 public:
  BigInt();

  BigInt(const int64_t& num);

  BigInt(const std::string& num);

  BigInt(const BigInt& num);

  ~BigInt();

  BigInt& operator+=(const BigInt& num);

  BigInt& operator=(const BigInt& num);

  BigInt& operator-=(const BigInt& num);

  BigInt& operator*=(const BigInt& num);

  BigInt& operator/=(const BigInt& num);

  BigInt& operator%=(const BigInt& num);

  BigInt operator-() const;

  BigInt& operator++();

  BigInt operator++(int);

  BigInt& operator--();

  BigInt operator--(int);

  friend bool operator<(const BigInt& first, const BigInt& second);

  friend BigInt operator+(const BigInt& first, const BigInt& second);

  friend BigInt operator-(const BigInt& first, const BigInt& second);

  friend BigInt operator*(const BigInt& first, const BigInt& second);

  friend BigInt operator/(const BigInt& first, const BigInt& second);

  friend BigInt operator%(const BigInt& first, const BigInt& second);

  friend std::istream& operator>>(std::istream& is_stream, BigInt& second);

  friend std::ostream& operator<<(std::ostream& os_stream,
                                  const BigInt& second);

 private:
  static const int kMaxNumOfDigits = 5;
  static const int64_t kMaxNumInDigit = 1e5 - 1;
  bool sign_ = true;
  std::vector<int64_t> array_;

  bool CompareModules(BigInt& first, const BigInt& second);

  BigInt* CasesForDevision(const BigInt& num);
  void HelperFunctionForDevision(int& size, BigInt& inter);
  BigInt& RealCountForSubstraction(const BigInt& num);
  BigInt& FirstCaseForSubstraction(const BigInt& num);
  BigInt& SecondCaseForSubstraction(const BigInt& num);
};

bool operator<(const BigInt& first, const BigInt& second);

bool operator>(const BigInt& first, const BigInt& second);

bool operator<=(const BigInt& first, const BigInt& second);

bool operator>=(const BigInt& first, const BigInt& second);

bool operator==(const BigInt& first, const BigInt& second);

bool operator!=(const BigInt& first, const BigInt& second);

BigInt operator+(const BigInt& first, const BigInt& second);

BigInt operator-(const BigInt& first, const BigInt& second);

BigInt operator*(const BigInt& first, const BigInt& second);

BigInt operator/(const BigInt& first, const BigInt& second);

BigInt operator%(const BigInt& first, const BigInt& second);

std::istream& operator>>(std::istream& is_stream, BigInt& second);

std::ostream& operator<<(std::ostream& os_stream, const BigInt& second);

int64_t Abs(const int64_t& num);