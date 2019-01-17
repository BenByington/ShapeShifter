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

#ifndef RENDERING_SHADERS_UNIFORM_VARIABLE_BASE_H
#define RENDERING_SHADERS_UNIFORM_VARIABLE_BASE_H

#include "rendering/shaders/InterfaceVariableBase.h"
#include "rendering/shaders/UniformManager.h"
#include "rendering/Camera.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

namespace detail {

struct manager_member_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(typename std::unique_ptr<typename T::UniformManager>{}, true) {
    return true;
  }
  static constexpr auto valid(...) { return false; }
};

struct initializer_member_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(typename std::unique_ptr<typename T::UniformInitializer>{}, true) {
    return true;
  }
  static constexpr auto valid(...) { return false; }
};

struct storage_member_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(typename std::unique_ptr<typename T::StorageType>{}, true) {
    return true;
  }
  static constexpr auto valid(...) { return false; }
};

struct combine_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::Combine, true) {
    using Type = decltype(&T::Combine);
    return std::is_same<Type, void (T::*)(const T&)>::value;
  }
  static constexpr bool valid(...) { return false; }
};

struct data_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::UniformManager::Data, true) {
    using ActualType = decltype(&T::UniformManager::Data);

    using ReturnType = typename T::UniformManager::StorageType;
    using ExpectedType = ReturnType (T::UniformManager::*)(const Camera&) const;
    return std::is_same<ActualType, ExpectedType>::value;
  }
  static constexpr bool valid(...) { return false; }
};

struct clone_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::Clone, true) {
    using Type = decltype(&T::Clone);
    return std::is_same<Type, void (T::*)(const T&)>::value;
  }
  static constexpr bool valid(...) { return false; }
};

template <class Result>
struct init_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::InitUniform, true) {
    using Type = decltype(&T::InitUniform);
    return std::is_same<Type, void (T::*) (Result&) const>::value;
  }
  static constexpr bool valid(...) { return false; }
};

}

template <class Child, typename T>
class UniformVariableBase : public InterfaceVariableBase<Child, T> {
  using Base = InterfaceVariableBase<Child, T>;
public:
  virtual ~UniformVariableBase() {
    constexpr Child* child = nullptr;
    static_assert(detail::manager_member_exists::valid(child),
        "Children of UniformVariableBase must must define an inner class called"
        " UniformManager");
    static_assert(detail::initializer_member_exists::valid(child),
        "Children of UniformVariableBase must must define an inner class called"
        " UniformInitializer");
    static_assert(detail::data_function_exists::valid(child),
        "UniformManager must have a function: T Data() const");

    constexpr typename Child::UniformManager* uniform = nullptr;
    static_assert(detail::storage_member_exists::valid(uniform),
        "UniformManager class must have a StorageType typedef "
        "declaring the data type it will return");
    // TODO how does the AntiCombine function fit with this?  Do we need new validation?
    //      I suspect not but need to verify
    static_assert(detail::combine_function_exists::valid(uniform),
        "UniformManager class must have function "
        "void Combine(const UniformManager&)");
    static_assert(std::is_default_constructible<typename Child::UniformManager>::value,
        "UniformManager class must be default constructable.");
    static_assert(detail::clone_function_exists::valid(uniform),
        "UniformManager must provide a Clone(UniformMananger&) function "
        "that enables deep copy");

    constexpr typename Child::UniformInitializer* initializer = nullptr;
    // TODO change to actually say signature
    static_assert(detail::init_function_exists<typename Child::UniformManager>::valid(initializer),
        "UniformInitializer class must provide a const named constructor called InitUniform");
  }
  using InterfaceVariableBase<Child, T>::InterfaceVariableBase;

  void UniformDeclaration(VariableFactory& factory) {
    factory.stream()
        << "uniform "
        << Base::Variable_T::TypeName()
        << " " << Child::name()
        << ";\n";
  }
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_INTERFACE_VARIABLE_BASE_H */

