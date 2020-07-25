/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Variable.h
 * Author: bbyington
 *
 * Created on November 15, 2016, 8:45 AM
 */

#ifndef RENDERING_SHADERS_LANGUAGE_VARIABLE_H
#define RENDERING_SHADERS_LANGUAGE_VARIABLE_H

#include "rendering/shaders/Pack.h"

#include <cassert>
#include <string>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

// forward declare
class VariableFactory;

namespace Language {

template<typename T>
class Expr;

namespace detail {

template <typename T>
struct extract_underlying {
  using type = typename std::decay<T>::type;
};
template <typename T>
struct extract_underlying<Expr<T>> {
  using type = T;
};

template <typename Is, typename Allowed>
struct AnyMatch;
template <typename Is, typename... Allowed>
struct AnyMatch<Is, Pack<Allowed...>> {
  template <bool... bs>
  static constexpr bool Any() {
    return (... || bs);
  }
  static constexpr bool Run() {
    static_assert((... || std::is_same<Is, Allowed>::value));
    return true;//Any(std::is_same<Is, Allowed>::value...);
  }
};

}

// Bases classes used for simplified type detection
struct VariableBase {};
struct ExprBase {};

template <typename ArithmeticPolicy>
class Variable : VariableBase {
public:
  using Policy = ArithmeticPolicy;
  Variable() : name_("crap"){}
  template <typename T1, typename... Ts>
  Variable(T1&& t1, Ts&&... types) {

    auto convert = [&](auto&& v)
    {
        using type = typename std::decay<decltype(v)>::type;
        if constexpr(std::is_floating_point<type>::value) {
          return v;
        } else if constexpr (std::is_base_of<VariableBase, type>::value) {
          return Expr<type>(v);
        } else if constexpr (std::is_base_of<ExprBase, type>::value) {
          return std::forward<decltype(v)>(v);
        } else {
          // can't use static_assert(false) because we need the condition to be a
          // dependant type, else this triggers always regardless of the types involved
          static_assert(!std::is_same<type, type>::value,
                        "Language::Variable conversion error");
        }
    };

    if constexpr (sizeof...(Ts) == 0 && std::is_same<typename detail::extract_underlying<T1>::type, Variable>::value) {
        //handle copy
        assert(false);
    } else if constexpr (std::is_convertible<T1, std::string>::value) {
      name_ = t1;
      Proc(convert(std::forward<Ts>(types))...);
    } else {
      name_ = "crap";
      Proc(convert(std::forward<T1>(t1)), convert(std::forward<Ts>(types))...);
    }
  }

  template <typename T1>
  void operator=(T1&& t) {
    using BaseType = typename std::decay<T1>::type;

    if constexpr(std::is_same<BaseType, Variable>::value) {
        assert(false);
      } else if constexpr(std::is_same<BaseType, Expr<Variable>>::value) {
        assert(false);
      }
    else
      static_assert(!std::is_same<BaseType, BaseType>::value);
  }

  Variable(const Variable&) = delete;
  Variable(Variable&&) = default;
  Variable& operator=(const Variable&) = delete;
  Variable& operator=(Variable&&) = default;

  ~Variable() = default;

  static constexpr const char* TypeName() { return Policy::TypeNameImpl(); }

  template <typename... Types>
  void Proc(Types... types) {
    using Signature = Pack<typename detail::extract_underlying<Types>::type...>;
    static_assert(detail::AnyMatch<Signature, typename Policy::CtorArgs>::Run(),
                  "Invalid Variable construction arguemnts");

    assert(false);
  }

private:
  std::string name_;
};

template<typename T>
class Expr<Variable<T>> : ExprBase {
public:
  Expr(const Variable<T>& v) {}
  Expr(Variable<T>&& v) {}
};

}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_VARIABLE_H */

