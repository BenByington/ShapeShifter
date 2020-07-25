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

struct accumulator_member_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(typename std::unique_ptr<typename T::UniformAccumulator>{}, true) {
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

template <class Arg>
struct combine_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::Combine, true) {
    using Type = decltype(&T::Combine);
    return std::is_same<Type, void (T::*)(const Arg&)>::value;
  }
  static constexpr bool valid(...) { return false; }
};

struct data_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::UniformAccumulator::Data, true) {
    using ActualType = decltype(&T::UniformAccumulator::Data);

    using ReturnType = typename T::UniformAccumulator::StorageType;
    using ExpectedType = ReturnType (T::UniformAccumulator::*)(const Camera&) const;
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
public:
  using Base = InterfaceVariableBase<Child, T>;

  // Destructor used to enforce expected features of UniformVariables.
  ~UniformVariableBase() {
    constexpr Child* child = nullptr;
    // Make sure the appropriate inner classes are present. UniformManager is for
    // setting per-node uniform information.  UniformAccumulator is for accumulating
    // UniformManagers over the rendering tree (for heirarchical things like transformations)
    // and UniformInitializers are for the initial conditions of accumulating over the tree.
    static_assert(detail::manager_member_exists::valid(child),
        "Children of UniformVariableBase must must define an inner class called"
        " UniformManager");
    static_assert(detail::accumulator_member_exists::valid(child),
        "Children of UniformVariableBase must must define an inner class called"
        " UniformAccumulator");
    static_assert(detail::initializer_member_exists::valid(child),
        "Children of UniformVariableBase must must define an inner class called"
        " UniformInitializer");

    // All of these types must be default constructable, as they are associate with an
    // aggregation class that will inherit from them via variadic template pack.
    static_assert(std::is_default_constructible<typename Child::UniformManager>::value,
        "UniformManager class must be default constructable.");
    static_assert(std::is_default_constructible<typename Child::UniformAccumulator>::value,
        "UniformAccumulator class must be default constructable.");
    static_assert(std::is_default_constructible<typename Child::UniformInitializer>::value,
        "UniformInitializer class must be default constructable.");

    // enforce the API expected for the UniformAccumulator, necessary for heirarchical
    // combination and upload to the shader
    constexpr typename Child::UniformAccumulator* accumulator = nullptr;
    static_assert(detail::data_function_exists::valid(child),
        "UniformAccumulator must have a function: T Data() const");
    static_assert(detail::storage_member_exists::valid(accumulator),
        "UniformManager class must have a StorageType typedef "
        "declaring the data type it will return");
    static_assert(
        detail::combine_function_exists<typename Child::UniformManager>::valid(accumulator),
        "UniformManager class must have function "
        "void Combine(const UniformManager&)");
    static_assert(detail::clone_function_exists::valid(accumulator),
        "UniformManager must provide a Clone(UniformMananger&) function "
        "that enables deep copy");

    // enforce the API expected for the UniformInitializer, necessary for creating
    // the initial Accumulator used for tree traversal
    constexpr typename Child::UniformInitializer* initializer = nullptr;
    static_assert(
        detail::init_function_exists<typename Child::UniformAccumulator>::valid(initializer),
        "UniformInitializer class must provide a const named constructor called InitUniform "
        "that accepts (and populates) a UniformAccumulator");
  }
  using InterfaceVariableBase<Child, T>::InterfaceVariableBase;

  void UniformDeclaration() {
    Language::Stream()
        << "uniform "
        << Base::Variable_T::TypeName()
        << " " << Child::name()
        << ";\n";
  }
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_INTERFACE_VARIABLE_BASE_H */

