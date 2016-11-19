/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   InterfaceVariableBase.h
 * Author: bbyington
 *
 * Created on October 24, 2016, 8:18 AM
 */

#ifndef RENDERING_SHADERS_INTERFACE_VARIABLE_BASE_H
#define RENDERING_SHADERS_INTERFACE_VARIABLE_BASE_H

#include "rendering/shaders/language/Types.h"
#include "rendering/shaders/language/Variable.h"
#include "rendering/shaders/VariableFactory.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

namespace detail {

struct name_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::name, true) {
    using Type = decltype(&T::name);
    return std::is_same<Type, const char* (*)()>::value;
  }
  static constexpr bool valid(...) { return false; }
};

struct declares_smooth {
  static constexpr bool is_bool(bool b) { return true; }
  template <typename T>
  static constexpr bool is_bool(T b) { return false; }
  template <class T>
  static constexpr auto valid(T*) ->decltype(T::smooth, true) {
    if (is_bool(T::smooth)) {
      return true;
    } else {
      return false;
    }
  }
  static constexpr bool valid(...) { return false; }
};

}

template <class Child, typename T>
struct InterfaceVariableBase {
protected:
  InterfaceVariableBase(VariableFactory& factory)
    : factory_(factory), var(factory.create<T>(Child::name())) {
    constexpr Child* temp = nullptr;
    static_assert(detail::name_function_exists::valid(temp),
        "Children of InterfaceVariableBase must supply a (preferably"
        "constexpr) static function called name that returns a char*");
    static_assert(
        Language::VariableTraits<T>::InterfaceAllowed
      , "Type not allowed in InterfaceVariable");
    static_assert(
        detail::declares_smooth::valid(temp)
      , "Children of InterfaceVariableBase must have a static constexpr bool "
        "indicating if the variable should be smoothed");
  };

public:
  using Type = T;
  using Variable_T = Language::Variable<T>;

  InterfaceVariableBase(const InterfaceVariableBase&) = default;
  InterfaceVariableBase(InterfaceVariableBase&&) = default;

  InterfaceVariableBase& operator=(const InterfaceVariableBase&) = default;
  InterfaceVariableBase& operator=(InterfaceVariableBase&&) = default;

  virtual ~InterfaceVariableBase() {};

  std::pair<const std::string, size_t>
  LayoutDeclaration(VariableFactory& factory, size_t idx) {
    factory.stream()
        << "layout (location = "
        << idx << ") in "
        << Variable_T::TypeName()
        << " " << Child::name()
        << ";\n";
    return {Child::name(), idx};
  }

  void UniformDeclaration(VariableFactory& factory) {
    factory.stream()
        << "uniform "
        << Variable_T::TypeName()
        << " " << Child::name()
        << ";\n";
  }

  void OutputDeclaration(VariableFactory& factory) {
    if (Child::smooth) factory.stream() << "smooth ";
    factory.stream()
        << "out "
        << Variable_T::TypeName()
        << " " << Child::name()
        << ";\n";
  }

  void InputDeclaration(VariableFactory& factory) {
    if (Child::smooth) factory.stream() << "smooth ";
    factory.stream()
        << "in "
        << Variable_T::TypeName()
        << " " << Child::name()
        << ";\n";
  }

protected:
  Variable_T var;
  std::reference_wrapper<VariableFactory> factory_;
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_INTERFACE_VARIABLE_BASE_H */

