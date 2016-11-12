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

#include <cassert>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <type_traits>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

class VariableFactory;
class Vec3 {};
class Mat4 {};

class IndentedStringStream {
public:
  IndentedStringStream() = default;
  IndentedStringStream(const IndentedStringStream&) = delete;
  IndentedStringStream(IndentedStringStream&&) = default;
  IndentedStringStream& operator=(const IndentedStringStream&) = delete;
  IndentedStringStream& operator=(IndentedStringStream&&) = default;

  template <class T>
  std::stringstream& operator<<(T&& t) {
    for (size_t i = 0; i < indentation_; ++i) stream_ << " ";
    stream_ << t;
    return stream_;
  }

  // TODO delete?
  std::string str() {
    return stream_.str();
  }
private:
  std::stringstream stream_;
  size_t indentation_ = 0;
};

template <typename T>
class Expression {
public:
  //  TODO can this be protected somehow?  Child class needs to be able to
  //  create instances, but for some reason I can't.
  Expression(IndentedStringStream& stream, const std::string& name)
    : stream_(stream)
    , state_(name) {}
protected:
  Expression(const Expression&) = delete;
  Expression& operator=(const Expression&) = delete;
  Expression& operator=(Expression&&) = delete;

  std::reference_wrapper<IndentedStringStream> stream_;
  std::string state_;

public:
  Expression(Expression&&) = default;
  virtual ~Expression() {
    if(!state_.empty()) stream_.get() << state_;
  }
};

template <typename T>
class Variable : public Expression<T> {
  friend class VariableFactory;
  Variable(IndentedStringStream& stream, const std::string& name)
    : Expression<T>(stream, name) {}
public:
  Variable(const Variable&) = delete;
  Variable(Variable&&) = default;

  // Allow any implicit conversions?
  template <typename U>
  Expression<T> operator=(Expression<U>&& other) {
    // TODO precidence for other operators?
    std::string result = this->state_ + " = " + other.state_;
    other.state_.clear();
    return Expression<T>(this->stream_, result);
  }

  Expression<T> operator=(const Variable<T>& other) {
    std::string result = this->state_ + " = " + other.state_;
    return Expression<T>(this->stream_, result);
  }

  static constexpr const char* TypeName() {
    if (std::is_same<float, T>::value) {
      return "float";
    } else if (std::is_same<int, T>::value) {
      return "int";
    } else if (std::is_same<Vec3, T>::value) {
      return "vec3";
    } else if (std::is_same<Mat4, T>::value) {
      return "mat4";
    } else {
      return "Monsters in the soup!\n";
    }
  }
private:
};

class VariableFactory {
public:
  template <typename T>
  Variable<T> create(const std::string& name) {
    return Variable<T>(s, name);
  }

  IndentedStringStream& stream() { return s; };

private:
  IndentedStringStream s;
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
  InterfaceVariableBase(VariableFactory& factory)
    : factory_(factory), var(factory.create<T>(Child::name())) {
    constexpr Child* temp = nullptr;
    static_assert(detail::name_function_exists::valid(temp),
        "Children of InterfaceVariableBase must supply a (preferably"
        "constexpr) static function called name that returns a char*");
    static_assert(std::is_arithmetic<T>::value || std::is_same<Vec3, T>::value || std::is_same<Mat4, T>::value,
        "Children of InterfaceVariableBase have an arithmetic type");
    static_assert(detail::declares_smooth::valid(temp),
        "Children of InterfaceVariableBase must have a static constexpr bool "
        "indicating if the variable should be smoothed");
  };

public:
  using Type = T;
  using Variable_T = Variable<T>;

  InterfaceVariableBase(const InterfaceVariableBase&) = default;
  InterfaceVariableBase(InterfaceVariableBase&&) = default;

  InterfaceVariableBase& operator=(const InterfaceVariableBase&) = default;
  InterfaceVariableBase& operator=(InterfaceVariableBase&&) = default;

  virtual ~InterfaceVariableBase() {};

  void LayoutDeclaration(VariableFactory& factory, size_t idx) {
    // TODO save the idx.  Only the raw text version needs to parse the string
    factory.stream() << "layout (location = " << idx << ") " << Variable<Type>::TypeName() << " " << Child::name()<< ";\n";
  }

  void UniformDeclaration(VariableFactory& factory) {
    factory.stream() << "uniform " << Variable<Type>::TypeName() << " " << Child::name() << ";\n";
  }

  void OutputDeclaration(VariableFactory& factory) {
    if (Child::smooth) factory.stream() << "smooth ";
    factory.stream() << "out " << Variable<Type>::TypeName() << " " << Child::name() << ";\n";
  }

  // TODO unify with output?
  void InputDeclaration(VariableFactory& factory) {
    if (Child::smooth) factory.stream() << "smooth ";
    factory.stream() << "in " << Variable<Type>::TypeName() << " " << Child::name() << ";\n";
  }

protected:
  Variable_T var;
  std::reference_wrapper<VariableFactory> factory_;
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_INTERFACE_VARIABLE_BASE_H */

