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

#include "data/AbstractBufferTypes.h"
#include "data/BufferTypes.h"
#include "data/MixedDataMap.h"
#include "rendering/RenderNode.h"

#include <cstdlib>
#include <type_traits>

namespace ShapeShifter {
namespace Data {

namespace detail {

struct type_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(typename std::is_same<float, typename T::Type>::type{}, true) {
    using Type = typename T::Type;
    return std::is_same<float, Type>::value || std::is_same<uint32_t, Type>::value;
  }
  static constexpr bool valid(...) { return false; }
};

struct interface_function_exists {
  template <class T>
  static constexpr auto valid(T*) -> decltype(&T::Interface::FillData, true) {
    using Type = decltype(&T::Interface::FillData);
    return std::is_same<Type, void (T::Interface::*)(VectorSlice<typename T::Type>&)>::value;
  }
  static constexpr bool valid(...) { return false; }
};

}

template <class Child>
class BaseManager : public AbstractManager {
public:
  virtual ~BaseManager() {
    constexpr Child* temp = nullptr;
    static_assert(detail::type_exists::valid(temp),
        "Children of BaseManager must have either float or uin32_t defined as 'Type'");
    static_assert(detail::interface_function_exists::valid(temp),
        "Children of BaseManager must define a type Interface for Render nodes to inherit, "
        "which must have a function with the signature void FillData(VectorSlice<T>&) ");
  }

  template <class Child_>
  struct Dispatch {
    static void FillData(VectorSlice<typename Child_::Type>& data, Rendering::RenderNode* node) {
      assert(false);
      auto typed_node = dynamic_cast<typename Child_::Interface*>(node);
      typed_node->FillData(data);
    }
    static void FillData(...) {} //TODO comment
  };

  virtual void FillData(VectorSlice<float>& data, Rendering::RenderNode* node) override {
    Dispatch<Child>::FillData(data, node);
  }
  virtual void FillData(VectorSlice<uint32_t>& data, Rendering::RenderNode* node) override {
    Dispatch<Child>::FillData(data, node);
  }
};

class ColorManager : public BaseManager<ColorManager> {
public:
  using Type = float;
  virtual ~ColorManager(){}

  virtual size_t ElementsPerEntry() override { return 3; }
  virtual SupportedBufferFlags flag() override { return SupportedBufferFlags::COLORS; }
  virtual SupportedBuffers buffer() override { return SupportedBuffers::COLORS; }

  class Interface {
  public:
    void FillData(VectorSlice<Type>& data) { FillColorData(data); }
	  virtual void FillColorData(Data::VectorSlice<float>& data) const = 0;
  };

};

static ColorManager temp;

}} // ShapeShifter::Data

#endif /* DATA_BUFFERTYPES_H */

