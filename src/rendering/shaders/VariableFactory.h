/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   VariableFactory.h
 * Author: bbyington
 *
 * Created on November 16, 2016, 8:35 AM
 */

#include "rendering/shaders/language/IndentedStringStream.h"
#include "rendering/shaders/language/Variable.h"

#include <type_traits>
#include <vector>

#ifndef RENDERING_SHADERS_LANGUAGE_VARIABLE_FACTORY_H
#define RENDERING_SHADERS_LANGUAGE_VARIABLE_FACTORY_H

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

namespace detail {

using namespace Language;

template <typename... Args>
struct can_create {
  template <typename T>
  static constexpr auto valid(T*) -> decltype(&T::Create, true) {
    using Type = decltype(&T::Create);
    return std::is_same<Type, T (*)(Args...)>::value;
  }
  static bool constexpr valid(...) { return false; }
};

template <typename T>
struct strip_variable {
  using Type = T;
};
template <typename T>
struct strip_variable<Variable<T>> {
  using Type = T;
};
template <typename T>
struct strip_variable<Expression<T>> {
  using Type = T;
};

template <typename T>
struct extract {
  static std::string name(const T& t) {
    return std::to_string(t);
  }
};

template <typename T>
struct extract<Variable<T>>{
  static std::string name(const Variable<T>& t) {
    return t.state_;
  }
};

template <typename T>
struct extract<Expression<T>>{
  static std::string name(const Expression<T>& t) {
    return t.state_;
  }
};

}

class VariableFactory {
public:
  template <typename T>
  decltype(auto) create(const std::string& name) {
    return Language::Variable<T>(s, name);
  }

  template <typename T, typename... Args>
  Language::Expression<T> temporary(Args&&... args) {
    static_assert(
        detail::can_create<
            // std::decay may potentially not be what we want?  Does things to arrays
            typename detail::strip_variable<typename std::decay<Args>::type>::Type...
        >::valid((T*)nullptr),
        "Cannot instantiate Expression<T> unless T defines a static"
        " `Create` function with the appropriate arguments\n");

    using Type = Language::Expression<T>;
    std::vector<std::string> names {
      detail::extract<typename std::decay<Args>::type>::name(args)...};
    std::string argument;
    for (size_t i = 0; i < names.size() - 1; ++i) {
      argument += names[i] + ", ";
    }
    if (names.size() > 0) {
      argument += names.back();
    }
    return Type(s, std::string(Type::TypeName()) + "(" + argument + ")", Type::Key());
  }

  Language::IndentedStringStream& stream() { return s; };

private:
  Language::IndentedStringStream s;
};


}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_LANGUAGEVVARIABLE_FACTORY_H */

