#pragma once
#include <iterator>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class List {
  struct BaseNode;
  struct Node;

  using allocator_for_node =
      typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

  using allocator_traits = std::allocator_traits<allocator_for_node>;

 public:
  using value_type = T;
  using allocator_type = Allocator;

  List() {}

  List(size_t count, const T& value, const Allocator& alloc = Allocator())
      : alloc_{alloc} {
    size_t i_idx = 0;
    try {
      for (; i_idx < count; ++i_idx) {
        push_back(value);
      }
    } catch (...) {
      for (size_t j = 0; j < i_idx; ++j) {
        pop_back();
      }
      throw;
    }
  }

  explicit List(size_t count, const Allocator& alloc = Allocator())
      : alloc_{alloc} {
    size_t i_idx = 0;
    try {
      for (; i_idx < count; ++i_idx) {
        push_back();
      }
    } catch (...) {
      for (size_t j = 0; j < i_idx; ++j) {
        pop_back();
      }
      throw;
    }
  }

  List(const List& other) {
    alloc_ =
        allocator_traits::select_on_container_copy_construction(other.alloc_);
    Node* intermediate = other.begin_;
    size_t i_idx = 0;
    try {
      for (; i_idx < other.size_; ++i_idx) {
        push_back(intermediate->value);
        intermediate = static_cast<Node*>(intermediate->right_neighbour);
      }
    } catch (...) {
      for (size_t j = 0; j < i_idx; ++j) {
        pop_back();
      }
      throw;
    }
  }

  List(std::initializer_list<T> init, const Allocator& alloc = Allocator())
      : alloc_{alloc} {
    auto iter = init.begin();
    size_t i_idx = 0;
    try {
      for (; i_idx < init.size(); ++i_idx) {
        push_back(*iter);
        ++iter;
      }
    } catch (...) {
      for (size_t j = 0; j < i_idx; ++j) {
        pop_back();
      }
      throw;
    }
  }

  List& operator=(const List& other) {
    auto alloc_copy = alloc_;
    auto begin_copy = begin_;
    auto end_copy = end_;
    auto size_copy = size_;
    auto fective_end_copy = fective_end_;
    if (allocator_traits::propagate_on_container_copy_assignment::value &&
        alloc_ != other.alloc_) {
      alloc_ = other.alloc_;
    }
    begin_ = nullptr;
    end_ = nullptr;
    fective_end_ = BaseNode();
    size_ = 0;
    Node* intermediate = other.begin_;
    size_t i_idx = 0;
    try {
      for (; i_idx < other.size_; ++i_idx) {
        push_back(intermediate->value);
        intermediate = static_cast<Node*>(intermediate->right_neighbour);
      }
    } catch (...) {
      for (size_t j = 0; j < i_idx; ++j) {
        pop_back();
      }
      std::swap(alloc_, alloc_copy);
      std::swap(begin_, begin_copy);
      std::swap(end_, end_copy);
      std::swap(size_, size_copy);
      std::swap(fective_end_, fective_end_copy);
      throw;
    }
    if (size_copy != 0) {
      intermediate = begin_copy;
      auto right_neighbour = intermediate->right_neighbour;
      for (size_t i_idx = 0; i_idx < size_copy; ++i_idx) {
        right_neighbour = intermediate->right_neighbour;
        allocator_traits::destroy(alloc_copy, intermediate);
        allocator_traits::deallocate(alloc_copy, intermediate, 1);
        intermediate = static_cast<Node*>(right_neighbour);
      }
    }
    return *this;
  }

  ~List() {
    size_t size = size_;
    for (size_t i_idx = 0; i_idx < size; ++i_idx) {
      pop_back();
    }
  }

  T& front() { return begin_->value; }

  const T& front() const { return begin_->value; }

  T& back() { return end_->value; }

  const T& back() const { return end_->value; }

  bool empty() const { return size_ == 0; }

  size_t size() const { return size_; }

  void push_back(const T& value) {
    Node* new_node = get_node_with_value<const T&>(value);
    link_node_back(new_node);
  }

  void push_back() {
    Node* new_node = get_node_with_value<>();
    link_node_back(new_node);
  }

  void push_back(T&& value) {
    Node* new_node = get_node_with_value<T&&>(std::move(value));
    link_node_back(new_node);
  }

  void pop_back() {
    if (size_ == 1) {
      allocator_traits::destroy(alloc_, end_);
      allocator_traits::deallocate(alloc_, end_, 1);
      end_ = nullptr;
      begin_ = nullptr;
    } else {
      end_ = static_cast<Node*>(end_->left_neighbour);
      allocator_traits::destroy(alloc_,
                                static_cast<Node*>(end_->right_neighbour));
      allocator_traits::deallocate(
          alloc_, static_cast<Node*>(end_->right_neighbour), 1);
      end_->right_neighbour = nullptr;
      update_fective_end();
    }
    --size_;
  }

  void push_front(const T& value) {
    Node* new_node = get_node_with_value(value);
    link_node_front(new_node);
  }

  void push_front(T&& value) {
    Node* new_node = get_node_with_value(value);
    link_node_front(new_node);
  }

  void pop_front() {
    if (size_ == 1) {
      allocator_traits::destroy(alloc_, begin_);
      allocator_traits::deallocate(alloc_, begin_, 1);
      begin_ = nullptr;
      end_ = nullptr;
    } else {
      begin_ = static_cast<Node*>(begin_->right_neighbour);
      allocator_traits::destroy(alloc_,
                                static_cast<Node*>(begin_->left_neighbour));
      allocator_traits::deallocate(
          alloc_, static_cast<Node*>(begin_->left_neighbour), 1);
      begin_->left_neighbour = nullptr;
    }
    --size_;
  }

  allocator_for_node get_allocator() { return alloc_; }

  template <bool IsConst>
  class BaseIterator {
   public:
    using value_type = typename std::conditional<IsConst, const T, T>::type;
    using pointer = typename std::conditional<IsConst, const T*, T*>::type;
    using reference = typename std::conditional<IsConst, const T&, T&>::type;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    BaseIterator(const BaseIterator& other) : node_(other.node_) {}

    BaseIterator& operator=(const BaseIterator& other) {
      BaseIterator copy = other;
      swap(*this, copy);
      return *this;
    }

    void swap(BaseIterator& first, BaseIterator& second) {
      std::swap(first.node_, second.node_);
    }

    BaseIterator(
        typename std::conditional_t<IsConst, const BaseNode*, BaseNode*> node)
        : node_(node) {}

    operator BaseIterator<true>() const { return const_iterator(node_); }

    typename std::conditional_t<IsConst, const T&, T&> operator*() const {
      return static_cast<
                 typename std::conditional_t<IsConst, const Node*, Node*>>(
                 node_)
          ->value;
    }

    typename std::conditional_t<IsConst, const T*, T*> operator->() const {
      return &static_cast<
                  typename std::conditional_t<IsConst, const Node*, Node*>>(
                  node_)
                  ->value;
    }

    BaseIterator& operator++() {
      node_ = node_->right_neighbour;
      return *this;
    }

    BaseIterator& operator--() {
      node_ = node_->left_neighbour;
      return *this;
    }

    BaseIterator operator++(int) {
      BaseIterator copy = *this;
      ++(*this);
      return copy;
    }

    BaseIterator operator--(int) {
      BaseIterator copy = *this;
      --(*this);
      return copy;
    }

    bool operator==(const BaseIterator& other) const {
      return node_ == other.node_;
    }

    bool operator!=(const BaseIterator& other) const {
      return !(*this == other);
    }

   private:
    std::conditional_t<IsConst, const BaseNode*, BaseNode*> node_;
  };

  using iterator = BaseIterator<false>;

  using const_iterator = BaseIterator<true>;

  using reverse_iterator = std::reverse_iterator<iterator>;

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() { return iterator(begin_); }

  const_iterator begin() const { return const_iterator(begin_); }

  const_iterator cbegin() const { return const_iterator(begin_); }

  reverse_iterator rbegin() {
    return reverse_iterator(std::make_reverse_iterator(end()));
  }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(std::make_reverse_iterator(cend()));
  }

  const_iterator crbegin() const {
    return const_reverse_iterator(std::make_reverse_iterator(cend()));
  }

  iterator end() { return iterator(&fective_end_); }

  const_iterator end() const { return const_iterator(&fective_end_); }

  const_iterator cend() const { return const_iterator(&fective_end_); }

  reverse_iterator rend() {
    return reverse_iterator(std::make_reverse_iterator(begin()));
  }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(std::make_reverse_iterator(cbegin()));
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(std::make_reverse_iterator(cbegin()));
  }

 private:
  void link_node_back(Node* new_node) {
    if (is_null_ptr(end_)) {
      end_ = new_node;
      begin_ = new_node;
    } else {
      end_->right_neighbour = new_node;
      new_node->left_neighbour = end_;
      end_ = new_node;
    }
    ++size_;
    update_fective_end();
  }

  void link_node_front(Node* new_node) {
    if (is_null_ptr(begin_)) {
      begin_ = new_node;
      end_ = new_node;
    } else {
      begin_->left_neighbour = new_node;
      new_node->right_neighbour = begin_;
      begin_ = new_node;
    }
    ++size_;
  }

  bool is_null_ptr(Node* ptr) { return ptr == nullptr; }

  template <typename... Args>
  Node* get_node_with_value(Args&&... args) {
    Node* new_node = nullptr;
    new_node = allocator_traits::allocate(alloc_, 1);
    try {
      allocator_traits::construct(alloc_, new_node,
                                  std::forward<Args>(args)...);
    } catch (...) {
      allocator_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
    return new_node;
  }

  void update_fective_end() {
    fective_end_.left_neighbour = end_;
    end_->right_neighbour = &fective_end_;
  }

  struct BaseNode {
    BaseNode() = default;
    BaseNode* left_neighbour = nullptr;
    BaseNode* right_neighbour = nullptr;
  };

  struct Node : BaseNode {
    Node() : value{T()} {}
    Node(const T& value) : value{value} {}
    Node(const Node& other) {
      Node copy = other;
      std::swap(*this, copy);
    }
    T value;
  };

  allocator_for_node alloc_;
  Node* begin_ = nullptr;
  Node* end_ = nullptr;
  BaseNode fective_end_;
  size_t size_ = 0;
};