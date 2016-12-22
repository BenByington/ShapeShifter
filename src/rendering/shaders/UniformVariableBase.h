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
    using Type = decltype(&T::UniformManager::Data);
    using ReturnType = typename T::UniformManager::StorageType;
    return true;
    // TODO: enable this again once we no longer have to hand in a Camera
    //       to this function
    //return std::is_same<Type, ReturnType (T::UniformManager::*)()>::value;
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

}

template <class Child, typename T>
class UniformVariableBase : public InterfaceVariableBase<Child, T> {
  using Base = InterfaceVariableBase<Child, T>;
public:
  virtual ~UniformVariableBase() {
    constexpr Child* child = nullptr;
    static_assert(detail::manager_member_exists::valid(child),
        "Children of UniformVariableMust must define an inner class called"
        " UniformManager");
    constexpr typename Child::UniformManager* uniform = nullptr;
    static_assert(detail::storage_member_exists::valid(uniform),
        "UniformManager class must have a StorageType typedef "
        "declaring the data type it will return");
    static_assert(detail::combine_function_exists::valid(uniform),
        "UniformManager class must have function "
        "void Combine(const UniformManager&)");
    static_assert(std::is_default_constructible<typename Child::UniformManager>::value,
        "UniformManager class must be default constructable.");
    static_assert(detail::data_function_exists::valid(child),
        "UniformManager must have a function: T Data()");
    static_assert(detail::clone_function_exists::valid(uniform),
        "UniformManager must provide a Clone(UniformMananger&) function "
        "that enables deep copy");
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

