#pragma once

namespace raii {

template <typename T> class AutoPtr {
public:
  AutoPtr(T *ptr = nullptr) : ptr_(ptr) {}

  ~AutoPtr() { delete ptr_; }

  AutoPtr(AutoPtr &a) = delete;

  AutoPtr(AutoPtr &&a) noexcept : ptr_(a.ptr_) { a.ptr_ = nullptr; }

  AutoPtr &operator=(AutoPtr &a) = delete;

AutoPtr&operator=(AutoPtr&&a) noexcept {
      if (&a == this) {
        return *this;
      }

      delete ptr_;

      ptr_ = a.ptr_;
      a.ptr_ = nullptr;

      return *this;
  }

  T &operator*() const { return *ptr_; }

  T *operator->() const { return ptr_; }

  bool isNull() { return ptr_ == nullptr; }

private:
  T *ptr_{};
};
} // namespace raii