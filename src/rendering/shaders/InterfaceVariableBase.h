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

template <typename T>
class Variable {
  friend class VariableFactory;
  Variable(std::stringstream& stream) : stream_(stream) {}
public:
  Variable(const Variable&) = delete;
  Variable(Variable&&) = delete;
  Variable& operator=(const Variable&) = delete;
  Variable& operator=(Variable&&) = delete;

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

}

template <class Child, typename T>
struct InterfaceVariableBase {
protected:
  InterfaceVariableBase() {
    constexpr Child* temp = nullptr;
    static_assert(detail::name_function_exists::valid(temp),
        "Children of InterfaceVariableBase must supply a (preferably"
        "constexpr) static function called name that returns a char*");
    static_assert(std::is_arithmetic<T>::value,
        "Children of InterfaceVariableBase have an arithmetic type");
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
    // TODO vec3 needs to be configurable
    // TODO save the idx.  Only the raw text version needs to parse the string
    factory.stream() << "layout (location = " << idx << ") vec3 " << Child::name()<< ";\n";
  }

  void OutputDeclaration(VariableFactory& factory) {
    // TODO fix vec3 and make 'smooth' configurable
    factory.stream() << "smooth out vec3 " << Child::name() << ";\n";
  }

protected:
  std::unique_ptr<Rendering::Shaders::Variable<Type>> var;
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_INTERFACE_VARIABLE_BASE_H */

