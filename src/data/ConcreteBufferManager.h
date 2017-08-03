/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BufferTypes.h
 * Author: bbyington
 *
 * Created on September 14, 2016, 4:00 PM
 */

#ifndef DATA_CONCRETE_BUFFERTYPES_H
#define DATA_CONCRETE_BUFFERTYPES_H

#include "data/AbstractBufferManager.h"
#include "data/MixedDataMapBase.h"
#include "rendering/shaders/InterfaceVariableBase.h"

#include <cstdlib>
#include <type_traits>

namespace ShapeShifter {
namespace Data {

namespace detail {

// Helper class, used to guarantee that any buffer manager defined declares if
// it utilizes floating point or integral data.
struct type_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(typename std::is_same<float, typename T::Type>::type{}, true) {
    using Type = typename T::Type;
    return std::is_same<float, Type>::value || std::is_same<uint32_t, Type>::value;
  }
  static constexpr bool valid(...) { return false; }
};

// Helper class, used to guarantee that any buffer manager defined supplies an
// inner class named Interface that has a FillData function.  It is assumed but
// not enforced that this FillData function will in turn dispatch to a uniquely
// named virtual function that RenderNode children classes will implement.
// e.g. A RenderNode that supports the VertexManager will extend the
// VertexManager::Interface class and be forced to implement the virtual
// FillVertexData() function.  The BaseManager class will do a dynamic cast
// under this assumption.
struct interface_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::Interface::FillData, true) {
    using Type = decltype(&T::Interface::FillData);
    return std::is_same<Type, void (T::Interface::*)(VectorSlice<typename T::Type>&)>::value;
  }
  static constexpr bool valid(...) { return false; }
};

struct variable_member_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(typename std::unique_ptr<typename T::Variable>{}, true) {
    using Parent = Rendering::Shaders::InterfaceVariableBase<
        typename T::Variable,
        typename T::Type2>;
    return std::is_base_of<Parent, typename T::Variable>::value;
  }
  static constexpr auto valid(...) { return false; }
};

}

/*
 * CRTP base class, which all concrete buffer managers should extend from.
 * The CRTP nature of this class allows it to know the full type of the
 * final class, without having to write duplicate code in each of the children.
 */
template <class Child>
class BaseManager : public AbstractManager {
public:
  BaseManager() : AbstractManager() {}
  BaseManager(const BaseManager&) = delete;
  BaseManager(BaseManager&&) = delete;
  BaseManager& operator=(const BaseManager&) = delete;
  BaseManager& operator=(BaseManager&&) = delete;

  // Make sure that the Child class has the expected properties.  The
  // static_asserts are thrown in this destructor instead of at class scope,
  // because due to the CRTP patter, Child will not be fully defined at
  // that point.
  virtual ~BaseManager() {
    constexpr Child* temp = nullptr;
    static_assert(detail::type_exists::valid(temp),
        "Children of BaseManager must have either float or uint32_t defined as 'Type'");
    static_assert(detail::interface_function_exists::valid(temp),
        "Children of BaseManager must define a type Interface for Render nodes to inherit, "
        "which must have a function with the signature void FillData(VectorSlice<T>&) ");
    static_assert(detail::variable_member_exists::valid(temp),
        "Children of BaseManager must declare an inner class that extends"
        " InterfaceVariableBase and is named Variable, for use in the shader program");
  }

  // Helper class, so that both FillData functions can direct to here, and
  // only the correct (and expected) type will have an implementation
  template <class Child_>
  struct Dispatch {
    static void FillData(VectorSlice<typename Child_::Type>& data, Rendering::BaseLeafNode* node) {
      auto typed_node = dynamic_cast<typename Child_::Interface*>(node);
      assert(typed_node);
      typed_node->FillData(data);
    }
    // Dummy implementation, should never be called unless the types got
    // mixed up.  Just needed as we can't tell in this class which type
    // is used in this buffer storage.
    static void FillData(...) { assert(false); }
  };

  virtual std::string name() const override {
    return Child::Variable::name();
  }

  virtual void FillData(VectorSlice<float>& data, Rendering::BaseLeafNode* node) override {
    Dispatch<Child>::FillData(data, node);
  }
  virtual void FillData(VectorSlice<uint32_t>& data, Rendering::BaseLeafNode* node) override {
    Dispatch<Child>::FillData(data, node);
  }
};

class ColorManager final : public BaseManager<ColorManager> {
public:
  using Type = float;
  using Type2 = Rendering::Shaders::Language::Vec3;

  virtual ~ColorManager(){}

  virtual size_t ElementsPerEntry() const override { return 3; }
  virtual bool isFloating() const override { return true; }

  struct Variable : Rendering::Shaders::InterfaceVariableBase<Variable, Type2> {
    using Base = Rendering::Shaders::InterfaceVariableBase<Variable, Type2>;
    using Base::InterfaceVariableBase;
    static constexpr const char* name() {
      return "inColor";
    }
    static constexpr bool smooth = false;
    Variable_T& inColor = Base::var;
  };

  class Interface {
  public:
    void FillData(VectorSlice<Type>& data) { FillColorData(data); }
	  virtual void FillColorData(Data::VectorSlice<Type>& data) const = 0;
  };
};

class VertexManager final : public BaseManager<VertexManager> {
public:
  using Type = float;
  using Type2 = Rendering::Shaders::Language::Vec3;
  virtual ~VertexManager(){}

  virtual size_t ElementsPerEntry() const override { return 3; }
  virtual bool isFloating() const override { return true; }

  struct Variable : Rendering::Shaders::InterfaceVariableBase<Variable, Type2> {
    using Base = Rendering::Shaders::InterfaceVariableBase<Variable, Type2>;
    using Base::InterfaceVariableBase;
    static constexpr const char* name() {
      return "inPosition";
    }
    static constexpr bool smooth = false;
    Variable_T& inPosition = Base::var;
  };

  class Interface {
  public:
    void FillData(VectorSlice<Type>& data) { FillVertexData(data); }
	  virtual void FillVertexData(Data::VectorSlice<Type>& data) const = 0;
  };
};

}} // ShapeShifter::Data

#endif /* DATA_BUFFERTYPES_H */

