
#ifndef UTIL_MULTI_REFERENCE_WRAPPER_H
#define UTIL_MULTI_REFERENCE_WRAPPER_H

#include <concepts>
#include <functional>
#include <memory>

namespace ShapeShifter::Util {

namespace detail {

template <typename T>
struct aux_wrapper {
  aux_wrapper(T& t) : val{t} {}
  std::reference_wrapper<T> val;
};

template <typename T, typename... Parents>
concept InheritsFromAll = (std::derived_from<T, Parents> && ...);

template <typename T, typename... Set>
concept IsAnyOf = (std::same_as<T, Set> || ...);

}

/*
 * Used to create a non-owning reference_wrapper that can be called like a smart
 * pointer.  It has a main type T with normal semantics, but in the event of
 * type erasure with multiple inheritance, it can optionally provide access
 * to other parent types as well.
 */
template <class T, class...Other>
class MultiReferenceWrapper : private detail::aux_wrapper<Other>... {

public:
  template <detail::InheritsFromAll<T, Other...> U>
  explicit MultiReferenceWrapper(U& u)
      : detail::aux_wrapper<Other>(u)...
      , w_(u) {}

  MultiReferenceWrapper(const MultiReferenceWrapper&) = delete;
  MultiReferenceWrapper(MultiReferenceWrapper&&) = default;
  MultiReferenceWrapper& operator=(const MultiReferenceWrapper&) = delete;
  MultiReferenceWrapper& operator=(MultiReferenceWrapper&&) = default;

  T* operator->() { return &(w_.get()); }
  T& operator*() { return w_.get(); }
  operator T&() { return w_; }
  operator const T&() const { return w_; }

  // These really could be defined in the parents, but I can't figure
  // out how to get a variadic using statement to work when the
  // operator has a space in the name...
  template<detail::IsAnyOf<T, Other...> U>
  operator U&() {
    return static_cast<detail::aux_wrapper<U>*>(this)->val.get();
  }
  template<detail::IsAnyOf<T, Other...> U>
  operator const U&() const {
    return static_cast<const detail::aux_wrapper<U>*>(this)->val.get();
  }
private:
  std::reference_wrapper<T> w_;
};

template <class T, class...Other>
class MultiReferenceOwner : public MultiReferenceWrapper<T, Other...> {
public:
  template <detail::InheritsFromAll<T, Other...> U>
  MultiReferenceOwner(std::unique_ptr<U> u)
    : MultiReferenceWrapper<T, Other...>(*u)
    , owner_(std::move(u)) {}

  using MultiReferenceWrapper<T, Other...>::operator->;
private:
  std::unique_ptr<T> owner_;
};
}

#endif // UTIL_MULTI_REFERENCE_WRAPPER_H
