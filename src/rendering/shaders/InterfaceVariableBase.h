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

#include <cstdlib>
#include <memory>
#include <sstream>
#include <type_traits>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

class VariableFactory;
class Vec3 {};

template <typename T>
class Variable {
  friend class VariableFactory;
  Variable(std::stringstream& stream) : stream_(stream) {}
public:
  Variable(const Variable&) = delete;
  Variable(Variable&&) = delete;
  Variable& operator=(const Variable&) = delete;
  Variable& operator=(Variable&&) = delete;

  static constexpr const char* TypeName() {
    if (std::is_same<float, T>::value) {
      return "float";
    } else if (std::is_same<int, T>::value) {
      return "int";
    } else if (std::is_same<Vec3, T>::value) {
      return "vec3";
    } else {
      return "Monsters in the soup!\n";
    }
  }
private:
  std::reference_wrapper<std::stringstream> stream_;
  std::string state_;
  bool initialized_ = false;
  bool used_ = false;
};

class VariableFactory {
public:
  template <typename T>
  std::unique_ptr<Variable<T>> createUP() {
    return std::unique_ptr<Variable<T>>(new Variable<T>(s));
  }

  std::stringstream& stream() { return s; };

private:
  std::stringstream s;
};

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
  InterfaceVariableBase() {
    constexpr Child* temp = nullptr;
    static_assert(detail::name_function_exists::valid(temp),
        "Children of InterfaceVariableBase must supply a (preferably"
        "constexpr) static function called name that returns a char*");
    static_assert(std::is_arithmetic<T>::value || std::is_same<Vec3, T>::value,
        "Children of InterfaceVariableBase have an arithmetic type");
    static_assert(detail::declares_smooth::valid(temp),
        "Children of InterfaceVariableBase must have a static constexpr bool "
        "indicating if the variable should be smoothed");
  };

public:
  using Type = T;

  InterfaceVariableBase(const InterfaceVariableBase&) = default;
  InterfaceVariableBase(InterfaceVariableBase&&) = default;

  InterfaceVariableBase& operator=(const InterfaceVariableBase&) = default;
  InterfaceVariableBase& operator=(InterfaceVariableBase&&) = default;

  virtual ~InterfaceVariableBase() {};

  void LayoutDeclaration(VariableFactory& factory, size_t idx) {
    var = factory.createUP<Type>();
    // TODO save the idx.  Only the raw text version needs to parse the string
    factory.stream() << "layout (location = " << idx << ") " << Variable<Type>::TypeName() << " " << Child::name()<< ";\n";
  }

  void OutputDeclaration(VariableFactory& factory) {
    if (Child::smooth) factory.stream() << "smooth ";
    factory.stream() << "out " << Variable<Type>::TypeName() << " " << Child::name() << ";\n";
  }

protected:
  std::unique_ptr<Variable<Type>> var;
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_INTERFACE_VARIABLE_BASE_H */

