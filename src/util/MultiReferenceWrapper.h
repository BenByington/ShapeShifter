
#ifndef UTIL_MULTI_REFERENCE_WRAPPER_H
#define UTIL_MULTI_REFERENCE_WRAPPER_H

namespace ShapeShifter {
namespace Util {

namespace detail {

template <typename T>
struct aux_wrapper {
  aux_wrapper(T& t) : val{t} {}
  std::reference_wrapper<T> val;
};

}

/*
 * Used to create a non-owning reference_wrapper that can be called like a smart
 * pointer.  It has a main type T with normal semantics, but in the event of
 * type erasure with multiple inheritance, it can optionally provide access
 * to other parent types as well.
 */
template <class T, class...Other>
class MultiReferenceWrapper : private detail::aux_wrapper<Other>... {
  template <typename U>
  static constexpr bool validate_aux_types() {
    const std::array<bool, sizeof...(Other)> is_child = {{std::is_base_of<Other, U>::value...}};
    int sum = 0;
    for (int i = 0; i < sizeof...(Other); ++i) {
      if (is_child[i]) sum++;
    }
    return sum == sizeof...(Other);
  }
  template <typename U>
  static constexpr bool validate_conversion() {
    const std::array<bool, sizeof...(Other)> is_child = {{std::is_base_of<Other, U>::value...}} ;
    int sum = 0;
    for (int i = 0; i < sizeof...(Other); ++i) {
      if (is_child[i]) sum++;
    }
    return sum == 1;
  }

public:
  // If any compilation errors crop up here, change contraints to SFINAE
  template <typename U>
    explicit MultiReferenceWrapper(U& u) : detail::aux_wrapper<Other>(u)..., w_(u) {
    static_assert(std::is_base_of<T,U>::value, "Is not a child of main type");
    static_assert(validate_aux_types<U>(), "Is not a child of one or more auxiliary types");
  }

  MultiReferenceWrapper(const MultiReferenceWrapper&) = delete;
  MultiReferenceWrapper(MultiReferenceWrapper&&) = default;
  MultiReferenceWrapper& operator=(const MultiReferenceWrapper&) = delete;
  MultiReferenceWrapper& operator=(MultiReferenceWrapper&&) = default;

  T* operator->() { return &(w_.get()); }
  T& operator*() { return w_.get(); }
  operator T&() { return w_; }
  operator const T&() const { return w_; }

  template<typename U>
  MultiReferenceWrapper<U> Convert() {
    static_assert(validate_conversion<U>(), "is not valid auxiliary type");
    U& v = static_cast<detail::aux_wrapper<U>*>(this)->val.get();
    return MultiReferenceWrapper<U>(v);
  }
  template<typename U>
  MultiReferenceWrapper<const U> Convert() const {
    static_assert(validate_conversion<U>(), "is not valid auxiliary type");
    const U& v = static_cast<const detail::aux_wrapper<U>*>(this)->val.get();
    return MultiReferenceWrapper<const U>(v);
  }
private:
  std::reference_wrapper<T> w_;
};

}}

#endif // UTIL_MULTI_REFERENCE_WRAPPER_H
