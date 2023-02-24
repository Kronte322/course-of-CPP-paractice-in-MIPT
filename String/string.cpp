#include "string.hpp"

bool String::DoesMemAlloced() const { return static_cast<bool>(capacity_); }
void String::Allocation() {
  capacity_ = 1;
  if (string_ == nullptr) {
    char* buffer = new char[capacity_ + 1]{};
    string_ = buffer;
  }
}
void String::Reallocation() {
  capacity_ = size_ * 2;
  char* buffer = new char[capacity_ + 1]{};
  memcpy(buffer, string_, size_);
  delete[] string_;
  string_ = buffer;
}

bool String::DoesNeedReallocation() const { return (size_ == capacity_); }

String::String() {}

String::String(const size_t& size, const char kCharacter)
    : size_{size}, capacity_{size_}, string_{new char[capacity_ + 1]{}} {
  memset(string_, kCharacter, size);
}

String::String(const char* string)
    : size_{strlen(string)},
      capacity_{size_},
      string_{new char[capacity_ + 1]{}} {
  memcpy(string_, string, size_);
}

String::String(const char kSymbol)
    : capacity_{1}, string_{new char[capacity_ + 1]{}} {
  if (kSymbol != '\0') {
    size_ = 1;
  }
  string_[0] = kSymbol;
}

String::String(const String& other)
    : size_{other.size_},
      capacity_{other.capacity_},
      string_{new char[capacity_ + 1]{}} {
  memcpy(string_, other.string_, size_);
}

String& String::operator=(const String& str) {
  Clear();
  String intermidiate(str);
  this->Swap(intermidiate);
  return *this;
}

String& String::operator=(const char* string) {
  Clear();
  String intermidiate(string);
  this->Swap(intermidiate);
  return *this;
}

String& String::operator=(const char kSymbol) {
  Clear();
  if (!(kSymbol == '\0')) {
    size_ = 1;
  }
  capacity_ = 1;
  Reserve(capacity_);
  string_[0] = kSymbol;
  return *this;
}

String::~String() { delete[] string_; }
void String::Clear() {
  if (DoesMemAlloced()) {
    memset(string_, '\0', size_);
    size_ = 0;
  }
}

void String::PushBack(const char kCharacter) {
  if (DoesNeedReallocation()) {
    if (DoesMemAlloced()) {
      Reallocation();
    } else {
      Allocation();
    }
  }
  string_[size_] = kCharacter;
  ++size_;
}

void String::PopBack() {
  if (!Empty()) {
    string_[size_ - 1] = '\0';
    --size_;
  }
}

void String::Resize(const size_t& new_size, const char kCharacter /*= '\0'*/) {
  if (new_size <= size_) {
    if (DoesMemAlloced()) {
      memset(string_ + new_size, kCharacter, size_ - new_size);
    }
  } else {
    Reserve(new_size);
    memset(string_ + size_, kCharacter, new_size - size_);
  }
  size_ = new_size;
}

void String::Reserve(const size_t& new_cap) {
  if (new_cap > capacity_) {
    if (!DoesMemAlloced()) {
      Allocation();
    }
    capacity_ = new_cap;
    char* buffer = new char[capacity_ + 1]{};
    memcpy(buffer, string_, size_);
    delete[] string_;
    string_ = buffer;
  }
}

void String::ShrinkToFit() {
  if (DoesMemAlloced()) {
    if (capacity_ > size_) {
      char* buffer = new char[size_ + 1];
      buffer[size_] = '\0';
      memcpy(buffer, string_, size_);
      delete[] string_;
      capacity_ = size_;
      string_ = buffer;
    }
  }
}

void String::Swap(String& other) {
  std::swap(string_, other.string_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
}

char& String::operator[](size_t idx) { return string_[idx]; }

const char& String::operator[](size_t idx) const { return string_[idx]; }

char& String::Front() { return string_[0]; }

const char& String::Front() const { return string_[0]; }

char& String::Back() { return string_[size_ - 1]; }

const char& String::Back() const { return string_[size_ - 1]; }

bool String::Empty() const { return (size_ == 0); }

size_t String::Size() const { return size_; }

size_t String::Capacity() const { return capacity_; }

char* String::Data() { return string_; }

const char* String::Data() const { return string_; }

String& String::operator+=(const String& other) {
  if (!other.Empty()) {
    Reserve(size_ + other.size_);
    memcpy(string_ + size_, other.string_, other.size_);
    size_ += other.size_;
    return *this;
  }
  return *this;
}

String& String::operator+=(const char* other) {
  String buffer = other;
  *this += buffer;
  return *this;
}

String& String::operator+=(const char kOther) {
  String buffer = kOther;
  *this += buffer;
  return *this;
}

String String::operator*(size_t n) const {
  String res;
  res.Reserve(size_ * n);
  for (size_t i = 0; i < n; ++i) {
    res += *this;
  }
  return res;
}

String String::operator*=(size_t n) const {
  String res;
  res.Reserve(size_ * n);
  for (size_t i = 0; i < n; ++i) {
    res += *this;
  }
  return res;
}

std::vector<String> String::Split(const String& delim /*= " "*/) {
  std::vector<String> result_after_split = {};
  result_after_split.resize(size_ / 2 + 1);
  String buffer;
  size_t iterator_for_vector = 0;
  size_t i = 0;
  while (i < size_ + 1) {
    String intermidiate;
    intermidiate.Reserve(delim.size_);
    size_t j = 0;
    for (; j < delim.size_; ++j) {
      intermidiate += string_[i];
      ++i;
      if (!(string_[i - 1] == delim.string_[j])) {
        buffer += intermidiate;
        break;
      }
    }
    if (j == delim.size_ || i == size_ + 1) {
      if (buffer.Empty()) {
        buffer.Allocation();
        result_after_split[iterator_for_vector++].Swap(buffer);
      } else {
        result_after_split[iterator_for_vector++].Swap(buffer);
      }
    }
  }
  result_after_split.resize(iterator_for_vector);
  return result_after_split;
}

String String::Join(const std::vector<String>& strings) const {
  String result_after_join;
  for (size_t i = 0; i < strings.size(); ++i) {
    result_after_join += strings[i];
    if (i != strings.size() - 1) {
      result_after_join += *this;
    }
  }
  return result_after_join;
}

std::ostream& operator<<(std::ostream& os, const String& str) {
  if (!str.Empty()) {
    os << str.string_;
  }
  return os;
}

std::istream& operator>>(std::istream& is, String& str) {
  char buffer;
  buffer = is.get();
  while (buffer != EOF && isspace(buffer) == 0) {
    str.PushBack(buffer);
    buffer = is.get();
  }
  return is;
}

bool operator<(const String& first, const String& second) {
  if (first.Empty()) {
    return !second.Empty();
  }
  if (second.Empty()) {
    return false;
  }
  return (strcmp(first.Data(), second.Data()) < 0);
}

bool operator>(const String& first, const String& second) {
  return second < first;
}

bool operator<=(const String& first, const String& second) {
  return !(first > second);
}

bool operator>=(const String& first, const String& second) {
  return !(first < second);
}

bool operator==(const String& first, const String& second) {
  return (first >= second) && (first <= second);
}

bool operator!=(const String& first, const String& second) {
  return !(first == second);
}

String operator+(const String& first, const String& second) {
  String result(first);
  result += second;
  return result;
}