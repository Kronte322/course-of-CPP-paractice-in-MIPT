#pragma once
#include <string.h>

#include <iostream>
#include <vector>

const size_t kBufferSize = 1000;

class String {
 private:
  size_t size_ = 0;
  size_t capacity_ = 0;
  char* string_ = nullptr;
  bool DoesMemAlloced() const;
  void Allocation();
  void Reallocation();
  bool DoesNeedReallocation() const;

 public:
  String();

  String(const size_t& size, char character);

  String(const char* string);

  String(char symbol);

  String(const String& other);

  String& operator=(const String& str);

  String& operator=(const char* string);

  String& operator=(char symbol);

  ~String();

  void Clear();

  void PushBack(char character);

  void PopBack();

  void Resize(const size_t& new_size, char character = '\0');

  void Reserve(const size_t& new_cap);

  void ShrinkToFit();

  void Swap(String& other);
  char& operator[](size_t idx);

  const char& operator[](size_t idx) const;

  char& Front();

  const char& Front() const;

  char& Back();

  const char& Back() const;

  bool Empty() const;

  size_t Size() const;

  size_t Capacity() const;

  char* Data();

  const char* Data() const;

  String& operator+=(const String& other);

  String& operator+=(const char* other);

  String& operator+=(char other);

  friend bool operator<(const String& first, const String& second);

  friend std::ostream& operator<<(std::ostream& os, const String& str);

  friend std::istream& operator>>(std::istream& is, String& str);

  String operator*(size_t n) const;

  String operator*=(size_t n) const;

  std::vector<String> Split(const String& delim = " ");

  String Join(const std::vector<String>& strings) const;
};

std::ostream& operator<<(std::ostream& os, const String& str);

std::istream& operator>>(std::istream& is, String& str);

bool operator<(const String& first, const String& second);

bool operator>(const String& first, const String& second);

bool operator<=(const String& first, const String& second);

bool operator>=(const String& first, const String& second);

bool operator==(const String& first, const String& second);

bool operator!=(const String& first, const String& second);

String operator+(const String& first, const String& second);