/**
 * @file deque.hpp
 * @author yaishen
 * @date 05.01.2023
 */

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

const int kSizeOfArrays = 20;

struct PositionOfBounds {
  PositionOfBounds() = default;

  PositionOfBounds(int row, int column, bool side)
      : row{row}, column{column}, side{side} {}

  PositionOfBounds(const PositionOfBounds& other) = default;

  PositionOfBounds& operator=(const PositionOfBounds& other) {
    row = other.row;
    column = other.column;
    side = other.side;
    return *this;
  }

  PositionOfBounds& operator+=(int range) {
    if (side) {
      if (range < kSizeOfArrays - column) {
        column += range;
      } else {
        range -= kSizeOfArrays - column;
        ++row;
        row += range / kSizeOfArrays;
        final_shift(range, false, true);
      }
    } else {
      if (range <= column) {
        column -= range;
      } else {
        range -= column + 1;
        ++row;
        row += range / kSizeOfArrays;
        final_shift(range, true, true);
      }
    }
    return *this;
  }

  PositionOfBounds& operator-=(int range) {
    if (side) {
      if (range <= column) {
        column -= range;
      } else {
        range -= column + 1;
        --row;
        row -= range / kSizeOfArrays;
        final_shift(range, true, false);
      }
    } else {
      if (range < kSizeOfArrays - column) {
        column += range;
      } else {
        range -= kSizeOfArrays - column;
        --row;
        row -= range / kSizeOfArrays;
        final_shift(range, false, false);
      }
    }
    return *this;
  }

  void final_shift(int range, bool flag = true, bool second_flag = true) {
    if ((static_cast<int>((range / kSizeOfArrays) % 2 == 0) ^
         static_cast<int>(flag)) != 0) {
      column = kSizeOfArrays - 1;
      side = true ^ second_flag;
      column -= range % kSizeOfArrays;
    } else {
      column = 0;
      side = false ^ second_flag;
      column += range % kSizeOfArrays;
    }
  }

  ptrdiff_t operator-(const PositionOfBounds& other) const {
    ptrdiff_t res = 0;
    if (row > other.row) {
      res += kSizeOfArrays * (row - other.row - 1);
      if (side) {
        res += column;
      } else {
        res += kSizeOfArrays - column - 1;
      }
      if (other.side) {
        res += kSizeOfArrays - other.column;
      } else {
        res += other.column + 1;
      }
    } else if (row == other.row) {
      if (column >= other.column) {
        res += column - other.column;
      } else {
        return -1;
      }
    } else {
      return -1;
    }
    return res;
  }

  bool operator==(const PositionOfBounds& other) const {
    return row == other.row && column == other.column && side == other.side;
  }

  bool operator!=(const PositionOfBounds& other) const {
    return !operator==(other);
  }

  PositionOfBounds get_next_element(bool flag = true) const {
    PositionOfBounds result = *this;
    if (flag) {
      result += 1;
    } else {
      result.side = !result.side;
      result -= 1;
      result.side = !result.side;
    }
    return result;
  }

  PositionOfBounds get_prev_element(bool flag = true) const {
    PositionOfBounds result = *this;
    if (flag) {
      result -= 1;
    } else {
      result.side = !result.side;
      result += 1;
      result.side = !result.side;
    }
    return result;
  }

  int row = 0;
  int column = 0;
  bool side = false;
};

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
  using allocator_traits = std::allocator_traits<Allocator>;
  using allocator_for_vector =
      typename std::allocator_traits<Allocator>::template rebind_alloc<T*>;

 public:
  template <bool IsConst>
  class base_iterator;

  template <bool IsConst>
  friend class base_iterator;

  template <bool IsConst>
  class base_reverse_iterator;
  template <bool IsConst>
  friend class base_reverse_iterator;

  Deque() {}

  Deque(const Allocator& alloc) : alloc_{alloc}, main_array_{alloc} {}

  Deque(const Deque& other)
      : begin_{other.begin_},
        end_{other.end_},
        alloc_{allocator_traits::select_on_container_copy_construction(
            other.alloc_)},
        main_array_{allocator_traits::select_on_container_copy_construction(
            other.alloc_)} {
    if (!other.is_mem_alloced()) {
      return;
    }
    allocate_mem(other.main_array_.size());
    size_t i_idx = 0;
    try {
      for (; i_idx < other.size(); ++i_idx) {
        push_back(other[i_idx]);
      }
    } catch (...) {
      free_mem(i_idx);
      throw;
    }
  }

  Deque(size_t count, const Allocator& alloc = Allocator())
      : alloc_{alloc}, main_array_(alloc) {
    if (count != 0) {
      allocate_mem(2);
      size_t i_index = 0;
      try {
        for (; i_index < count; ++i_index) {
          push_back();
        }
      } catch (...) {
        free_mem(i_index);
        throw;
      }
    }
  }

  Deque(size_t count, const T& value, const Allocator& alloc = Allocator())
      : alloc_{alloc}, main_array_{alloc} {
    if (count != 0) {
      allocate_mem(2);
      size_t i_index = 0;
      try {
        for (; i_index < count; ++i_index) {
          push_back(value);
        }
      } catch (...) {
        free_mem(i_index);
        throw;
      }
    }
  }

  Deque(Deque&& other)
      : begin_{other.begin_}, end_{other.end_}, size_{other.size_} {
    main_array_ = std::move(other.main_array_);
    alloc_ = std::move(other.alloc_);
    other.size_ = 0;
    other.begin_ = PositionOfBounds();
    other.end_ = PositionOfBounds();
  }

  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator())
      : alloc_{alloc}, main_array_{alloc} {
    auto iter = init.begin();
    size_t i_index = 0;
    try {
      for (; i_index < init.size(); ++i_index) {
        push_back(*iter);
        ++iter;
      }
    } catch (...) {
      for (size_t j = 0; j < i_index; ++j) {
        pop_back();
      }
      throw;
    }
  }

  ~Deque() {
    if (!is_mem_alloced()) {
      return;
    }
    size_t copy_size = size();
    for (size_t i = 0; i < copy_size; ++i) {
      pop_back();
    }
    for (auto& iter : main_array_) {
      allocator_traits::deallocate(alloc_, iter, kSizeOfArrays);
    }
  }

  void swap(Deque& first, Deque& second) {
    std::swap(first.main_array_, second.main_array_);
    std::swap(first.alloc_, second.alloc_);
    std::swap(first.begin_, second.begin_);
    std::swap(first.end_, second.end_);
    std::swap(first.size_, second.size_);
  }

  Deque<T, Allocator>& operator=(const Deque& other) {
    auto alloc_copy = alloc_;
    auto begin_copy = begin_;
    auto end_copy = end_;
    auto size_copy = size_;
    auto main_array_copy = main_array_;
    if (allocator_traits::propagate_on_container_copy_assignment::value &&
        alloc_ != other.alloc_) {
      alloc_ = other.alloc_;
      main_array_ = std::vector<T*, allocator_for_vector>(other.alloc_);
    }
    begin_ = PositionOfBounds();
    end_ = PositionOfBounds();
    main_array_.clear();
    main_array_.shrink_to_fit();
    size_ = 0;
    size_t i_idx = 0;
    try {
      for (; i_idx < other.size_; ++i_idx) {
        push_back(other[i_idx]);
      }
    } catch (...) {
      for (size_t j = 0; j < i_idx; ++j) {
        pop_back();
      }
      clear_main_array(main_array_, 0, main_array_.size());
      std::swap(alloc_, alloc_copy);
      std::swap(begin_, begin_copy);
      std::swap(end_, end_copy);
      std::swap(size_, size_copy);
      std::swap(main_array_, main_array_copy);
      throw;
    }
    Deque interm(alloc_copy, main_array_copy, begin_copy, end_copy, size_copy);
    return *this;
  }

  Deque& operator=(Deque&& other) {
    Deque copy = std::move(other);
    swap(copy, *this);
    return *this;
  }

  size_t size() const { return size_; }

  bool empty() const { return !static_cast<bool>(size()); }

  T& operator[](int32_t position) { return *(begin() + position); }

  const T& operator[](int32_t position) const { return *(begin() + position); }

  T& at(size_t position) {
    if (position >= size()) {
      throw std::out_of_range("out of range");
    }
    return operator[](position);
  }

  const T& at(size_t position) const {
    if (position >= size()) {
      throw std::out_of_range("out of range");
    }
    return operator[](position);
  }

  void push_back(T&& value) {
    check_for_pushes();
    allocator_traits::construct(alloc_, get_pointer_on_position(end_),
                                std::move(value));
    end_ = end_.get_next_element();
    ++size_;
  }

  void push_back(const T& value) {
    check_for_pushes();
    allocator_traits::construct(alloc_, get_pointer_on_position(end_), value);
    end_ = end_.get_next_element();
    ++size_;
  }

  void push_back() {
    check_for_pushes();
    allocator_traits::construct(alloc_, get_pointer_on_position(end_));
    end_ = end_.get_next_element();
    ++size_;
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    check_for_pushes();
    allocator_traits::construct(alloc_, get_pointer_on_position(end_),
                                std::forward<Args>(args)...);
    end_ = end_.get_next_element();
    ++size_;
  }

  void pop_back() {
    end_ = end_.get_prev_element();
    allocator_traits::destroy(alloc_, get_pointer_on_position(end_));
    --size_;
  }

  void push_front(const T& value) {
    check_for_pushes();
    allocator_traits::construct(alloc_, get_pointer_on_position(begin_), value);
    begin_ = begin_.get_next_element(false);
    ++size_;
  }

  void push_front(T&& value) {
    check_for_pushes();
    allocator_traits::construct(alloc_, get_pointer_on_position(begin_),
                                std::move(value));
    begin_ = begin_.get_next_element(false);
    ++size_;
  }

  template <typename... Args>
  void emplace_front(Args&&... args) {
    check_for_pushes();
    allocator_traits::construct(alloc_, get_pointer_on_position(begin_),
                                std::forward<Args>(args)...);
    begin_ = begin_.get_next_element(false);
    ++size_;
  }

  void pop_front() {
    begin_ = begin_.get_prev_element(false);
    allocator_traits::destroy(alloc_, get_pointer_on_position(begin_));
    --size_;
  }

  template <bool IsConst>
  class base_iterator {
   public:
    using value_type = typename std::conditional<IsConst, const T, T>::type;
    using pointer = typename std::conditional<IsConst, const T*, T*>::type;
    using reference = typename std::conditional<IsConst, const T&, T&>::type;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;

    base_iterator(PositionOfBounds position,
                  typename std::conditional<IsConst, const Deque<T, Allocator>*,
                                            Deque<T, Allocator>*>::type deque)
        : deque_{deque}, position_{position} {}

    base_iterator(const base_iterator& other)
        : position_{other.get_position()}, deque_{other.get_deque()} {}

    operator base_iterator<true>() const {
      return const_iterator(position_, deque_);
    }

    base_iterator& operator=(const base_iterator& other) {
      position_ = other.position_;
      deque_ = other.deque_;
      return *this;
    }

    typename std::conditional<IsConst, const T&, T&>::type operator*() const {
      return *deque_->get_pointer_on_position(position_);
    }

    typename std::conditional<IsConst, const T*, T*>::type operator->() const {
      return deque_->get_pointer_on_position(position_);
    }

    base_iterator& operator++() {
      position_ = position_.get_next_element();
      return *this;
    }

    base_iterator& operator--() {
      position_ = position_.get_prev_element();
      return *this;
    }

    base_iterator operator++(int) {
      base_iterator copy = *this;
      operator++();
      return copy;
    }

    base_iterator operator--(int) {
      base_iterator copy = *this;
      operator--();
      return copy;
    }

    base_iterator& operator+=(int range) {
      position_ += range;
      return *this;
    }

    base_iterator& operator-=(int range) {
      position_ -= range;
      return *this;
    }

    base_iterator operator+(int range) const {
      base_iterator copy = *this;
      copy += range;
      return copy;
    }

    base_iterator operator-(int range) const {
      base_iterator copy = *this;
      copy -= range;
      return copy;
    }

    difference_type operator-(const base_iterator& other) const {
      return position_ - other.position_;
    }

    bool operator<(const base_iterator& other) const {
      return (*this - other) < 0;
    }

    bool operator>(const base_iterator& other) const { return other < *this; }

    bool operator<=(const base_iterator& other) const {
      return !operator>(other);
    }

    bool operator>=(const base_iterator& other) const {
      return !operator<(other);
    }

    bool operator==(const base_iterator& other) const {
      return !operator<(other) && !operator>(other);
    }

    bool operator!=(const base_iterator& other) const {
      return !operator==(other);
    }

    typename std::conditional<IsConst, const Deque<T, Allocator>*,
                              Deque<T, Allocator>*>::type
    get_deque() const {
      return deque_;
    }

    PositionOfBounds get_position() const { return position_; }

   private:
    PositionOfBounds position_;
    typename std::conditional<IsConst, const Deque<T, Allocator>*,
                              Deque<T, Allocator>*>::type deque_;
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;

  using reverse_iterator = std::reverse_iterator<base_iterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<base_iterator<true>>;

  iterator begin() { return iterator(get_position_for_begin(), this); }

  const_iterator begin() const { return cbegin(); }

  const_iterator cbegin() const {
    return const_iterator(get_position_for_begin(), this);
  }

  reverse_iterator rbegin() {
    return reverse_iterator(std::make_reverse_iterator(end()));
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(std::make_reverse_iterator(cend()));
  }

  iterator end() { return iterator(end_, this); }

  const_iterator end() const { return cend(); }

  const_iterator cend() const { return const_iterator(end_, this); }

  reverse_iterator rend() {
    return reverse_iterator(std::make_reverse_iterator(begin()));
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(std::make_reverse_iterator(cbegin()));
  }

  void insert(iterator iter, const T& value) {
    push_back(value);
    if (iter != end() - 1) {
      PositionOfBounds first_pos = end_.get_prev_element();
      PositionOfBounds second_pos = first_pos.get_prev_element();
      while (second_pos != iter.get_position().get_prev_element()) {
        std::swap(*get_pointer_on_position(first_pos),
                  *get_pointer_on_position(second_pos));
        first_pos = second_pos;
        second_pos = first_pos.get_prev_element();
      }
    }
  }

  void erase(iterator iter) {
    PositionOfBounds first_pos = iter.get_position();
    PositionOfBounds second_pos = first_pos.get_next_element();
    while (second_pos != end().get_position()) {
      std::swap(*get_pointer_on_position(first_pos),
                *get_pointer_on_position(second_pos));
      first_pos = second_pos;
      second_pos = first_pos.get_next_element();
    }
    pop_back();
  }

  Allocator get_allocator() { return alloc_; }

 private:
  Deque(Allocator& alloc, std::vector<T*, allocator_for_vector>& main_array,
        PositionOfBounds begin, PositionOfBounds end, size_t size)
      : alloc_{alloc},
        main_array_{main_array},
        begin_{begin},
        end_{end},
        size_{size} {}

  void check_for_pushes() {
    if (!is_mem_alloced()) {
      allocate_mem(2);
    }
    if (is_full()) {
      reallocation();
    }
  }

  PositionOfBounds get_position_for_begin() const {
    PositionOfBounds position = begin_;
    position.side = !position.side;
    if (size_ == 0) {
      return position;
    }
    return position.get_next_element();
  }

  void free_mem(size_t position) {
    size_t counter = 0;
    for (size_t i = 0; i < position; ++i) {
      pop_back();
    }
    clear_main_array(main_array_, 0, main_array_.size());
  }

  T* get_pointer_on_position(PositionOfBounds position) {
    return main_array_[position.row + main_array_.size() / 2] + position.column;
  }

  const T* get_pointer_on_position(PositionOfBounds position) const {
    return main_array_[position.row + main_array_.size() / 2] + position.column;
  }

  bool is_mem_alloced() const {
    return static_cast<bool>(main_array_.capacity());
  }

  void allocate_mem(size_t size) {
    begin_ = PositionOfBounds(-1, 0, true);
    end_ = PositionOfBounds(0, 0, true);
    main_array_.resize(size);
    allocate_part(main_array_, 0, main_array_.size());
  }

  bool is_full() const { return front_overflow() || back_overflow(); }

  bool front_overflow() const {
    return (begin_.row + static_cast<int>(main_array_.size()) / 2) < 0;
  }

  bool back_overflow() const {
    return (end_.row + static_cast<int>(main_array_.size() / 2)) >=
           static_cast<int>(main_array_.size());
  }

  void reallocation() {
    std::vector<T*, allocator_for_vector> new_array(
        main_array_.size() * 2, main_array_.get_allocator());
    size_t begin_of_data = new_array.size() / 2 - main_array_.size() / 2;
    std::copy(main_array_.begin(), main_array_.end(),
              new_array.begin() + begin_of_data);
    allocate_part(new_array, 0, begin_of_data);
    allocate_part(new_array, begin_of_data + main_array_.size(),
                  new_array.size());
    main_array_ = new_array;
  }

  void allocate_part(std::vector<T*, allocator_for_vector>& array, size_t begin,
                     size_t end) {
    try {
      for (; begin < end; ++begin) {
        array[begin] = allocator_traits::allocate(alloc_, kSizeOfArrays);
      }
    } catch (...) {
      clear_main_array(array, 0, begin);
      throw;
    }
  }

  void clear_main_array(std::vector<T*, allocator_for_vector>& array,
                        size_t begin, size_t end) {
    for (size_t j = begin; j < end; ++j) {
      allocator_traits::deallocate(alloc_, array[j], kSizeOfArrays);
    }
  }

  Allocator alloc_;
  std::vector<T*, allocator_for_vector> main_array_;
  PositionOfBounds begin_;
  PositionOfBounds end_;
  size_t size_ = 0;
};