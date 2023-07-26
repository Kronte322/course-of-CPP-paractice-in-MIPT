#pragma once
#include <memory>
#include <type_traits>

struct BaseControllBlock {
  virtual void delete_pointer() = 0;
  virtual void delete_block() = 0;
  virtual ~BaseControllBlock() = default;

  size_t counter_for_shared = 0;
  size_t counter_for_weak = 0;
};

template <typename T>
class SharedPtr {
 public:
  SharedPtr() {}

  SharedPtr(T* ptr) {
    if (ptr != nullptr) {
      pointer_ = ptr;
      data_ = new DefaultControllBlock<T>(ptr);
      ++data_->counter_for_shared;
    }
  }

  template <typename U>
  SharedPtr(U* ptr)
      : pointer_(static_cast<T*>(ptr)),
        data_(new DefaultControllBlock<U>(ptr)) {
    ++data_->counter_for_shared;
  }

  template <typename U, typename Deleter>
  SharedPtr(U* ptr, Deleter deleter)
      : pointer_(static_cast<T*>(ptr)),
        data_(new DefaultControllBlock<U, Deleter>(ptr, deleter)) {
    ++data_->counter_for_shared;
  }

  template <typename U, typename Deleter, typename Allocator>
  SharedPtr(U* ptr, Deleter deleter, Allocator allocator)
      : pointer_(static_cast<T*>(ptr)) {
    rebind<Allocator, DefaultControllBlock<U, Deleter, Allocator>>
        interm_alloc = allocator;

    data_ = std::allocator_traits<
        rebind<Allocator, DefaultControllBlock<U, Deleter, Allocator>>>::
        allocate(interm_alloc, 1);
    std::allocator_traits<
        rebind<Allocator, DefaultControllBlock<U, Deleter, Allocator>>>::
        construct(
            interm_alloc,
            static_cast<DefaultControllBlock<U, Deleter, Allocator>*>(data_),
            ptr, std::move(deleter), std::move(interm_alloc));
    ++data_->counter_for_shared;
  }

  SharedPtr(const SharedPtr<T>& other)
      : pointer_(other.pointer_), data_(other.data_) {
    ++data_->counter_for_shared;
  }

  SharedPtr(SharedPtr<T>&& other)
      : pointer_(other.pointer_), data_(other.data_) {
    other.pointer_ = nullptr;
    other.data_ = nullptr;
  }

  template <typename V, typename = std::enable_if_t<std::is_base_of_v<V, T>>>
  operator SharedPtr<V>() {
    return SharedPtr<V>(static_cast<V*>(pointer_), data_);
  }

  void swap(SharedPtr<T>& other) {
    auto* ptr = other.pointer_;
    auto* data = other.data_;
    other.pointer_ = pointer_;
    other.data_ = data_;
    pointer_ = ptr;
    data_ = data;
  }

  SharedPtr& operator=(const SharedPtr<T>& other) {
    auto copy = other;
    swap(copy);
    return *this;
  }

  SharedPtr& operator=(SharedPtr<T>&& other) {
    auto copy = std::move(other);
    swap(copy);
    return *this;
  }

  size_t use_count() const {
    if (data_ == nullptr) {
      return 0;
    }
    return data_->counter_for_shared;
  }

  T* get() const { return pointer_; }

  T& operator*() { return *pointer_; }

  const T& operator*() const { return *pointer_; }

  T* operator->() { return pointer_; }

  const T* operator->() const { return pointer_; }

  void reset() { auto copy = std::move(*this); }

  ~SharedPtr() {
    if (data_ != nullptr) {
      --data_->counter_for_shared;
      if (data_->counter_for_shared == 0) {
        if (data_->counter_for_weak == 0) {
          data_->delete_block();
        } else {
          data_->delete_pointer();
        }
      }
    }
  }

  template <typename U, typename... Args>
  friend SharedPtr<U> MakeShared(Args&&... args);

  template <typename U, typename Alloc, typename... Args>
  friend SharedPtr<U> AllocateShared(const Alloc& alloc, Args&&... args);

  template <typename U>
  friend class WeakPtr;

  template <typename U>
  friend class SharedPtr;

 private:
  template <typename V, typename U>
  using rebind = typename std::allocator_traits<V>::template rebind_alloc<U>;

  template <typename U, typename Deleter = std::default_delete<U>,
            typename Allocator = std::allocator<U>>
  struct DefaultControllBlock : public BaseControllBlock {
    DefaultControllBlock(
        U* pointer, Deleter del = Deleter(),
        rebind<Allocator, DefaultControllBlock<U, Deleter, Allocator>> alloc =
            Allocator())
        : pointer(pointer), deleter(del), allocator(alloc) {}

    void delete_pointer() override {
      deleter(pointer);
      pointer = nullptr;
    }

    void delete_block() override {
      auto alloc = std::move(allocator);
      std::allocator_traits<decltype(alloc)>::destroy(alloc, this);
      std::allocator_traits<decltype(alloc)>::deallocate(alloc, this, 1);
    }

    ~DefaultControllBlock() {
      if (pointer != nullptr) {
        deleter(pointer);
      }
    }

    U* pointer = nullptr;
    Deleter deleter;
    rebind<Allocator, DefaultControllBlock<U, Deleter, Allocator>> allocator;
  };

  template <typename U, typename Allocator = std::allocator<U>>
  struct MakeSharedControllBlock : public BaseControllBlock {
    template <typename... Args>
    MakeSharedControllBlock(Allocator alloc, Args&&... args)
        : allocator(std::move(alloc)) {
      ::new (reinterpret_cast<U*>(&object)) U(std::forward<Args>(args)...);
      pointer = reinterpret_cast<U*>(&object);
    }

    U* get_pointer() { return pointer; }

    void delete_pointer() override {
      pointer->~U();
      pointer = nullptr;
    }

    void delete_block() override {
      auto alloc = std::move(allocator);
      std::allocator_traits<decltype(alloc)>::destroy(alloc, this);
      std::allocator_traits<decltype(alloc)>::deallocate(alloc, this, 1);
    }

    ~MakeSharedControllBlock() {
      if (pointer != nullptr) {
        pointer->~U();
      }
    }

    alignas(U) char object[sizeof(U)];
    U* pointer = nullptr;
    rebind<Allocator, MakeSharedControllBlock<U, Allocator>> allocator;
  };

  template <typename U, typename Alloc = std::allocator<U>>
  SharedPtr(MakeSharedControllBlock<U, Alloc>* block)
      : pointer_(block->get_pointer()), data_(block) {
    ++data_->counter_for_shared;
  }

  SharedPtr(T* ptr, BaseControllBlock* data) : pointer_(ptr), data_(data) {
    ++data_->counter_for_shared;
  }

  BaseControllBlock* get_block() const { return data_; }

  T* pointer_ = nullptr;
  BaseControllBlock* data_ = nullptr;
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return SharedPtr<T>(
      new typename SharedPtr<T>::template MakeSharedControllBlock<T>(
          std::allocator<int>(), std::forward<Args>(args)...));
}

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> AllocateShared(const Alloc& alloc, Args&&... args) {
  typename SharedPtr<T>::template rebind<
      Alloc, typename SharedPtr<T>::template MakeSharedControllBlock<T, Alloc>>
      interm_alloc = alloc;

  auto* ptr = std::allocator_traits<typename SharedPtr<T>::template rebind<
      Alloc, typename SharedPtr<T>::template MakeSharedControllBlock<
                 T, Alloc>>>::allocate(interm_alloc, 1);
  std::allocator_traits<typename SharedPtr<T>::template rebind<
      Alloc, typename SharedPtr<T>::template MakeSharedControllBlock<
                 T, Alloc>>>::construct(interm_alloc, (ptr),
                                        std::move(interm_alloc),
                                        std::forward<Args>(args)...);
  return SharedPtr<T>(ptr);
}

template <typename T>
class WeakPtr {
 public:
  WeakPtr() {}

  WeakPtr(const WeakPtr& other) : data_(other.data_) {
    ++data_->counter_for_weak;
  }

  WeakPtr(const SharedPtr<T>& ptr) : data_(ptr.get_block()) {
    ++data_->counter_for_weak;
  }

  WeakPtr(WeakPtr&& other) : data_(other.data_) { other.data_ = nullptr; }

  void swap(WeakPtr& other) {
    auto* save = other->data_;
    other->data_ = data_;
    data_ = save;
  }

  WeakPtr& operator=(const WeakPtr& other) {
    auto copy = other;
    swap(copy);
  }

  WeakPtr& operator=(WeakPtr&& other) {
    auto copy = std::move(other);
    swap(copy);
  }

  ~WeakPtr() {
    --data_->counter_for_weak;
    if (data_->counter_for_weak == 0 && data_->counter_for_shared == 0) {
      data_->delete_block();
    }
  }

  bool expired() { return data_->counter_for_shared == 0; }

  SharedPtr<T> lock() { return SharedPtr<T>(data_); }

 private:
  BaseControllBlock* data_ = nullptr;
};
