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
  BaseManager(size_t idx) : AbstractManager(idx) {}
  BaseManager(const BaseManager&) = delete;
  BaseManager(BaseManager&&) = delete;
  BaseManager& operator=(const BaseManager&) = delete;
  BaseManager& operator=(BaseManager&&) = delete;

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
      auto typed_node = dynamic_cast<typename Child_::Interface*>(node);
      assert(typed_node);
      typed_node->FillData(data);
    }
    // Dummy implementation, should never be called unless the types got
    // mixed up.  Just needed as we can't tell in this class which type
    // is used in this buffer storage.
    static void FillData(...) { assert(false); }
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
  static constexpr char key[] = "inColor";

  ColorManager(size_t idx) : BaseManager<ColorManager>(idx) {}
  virtual ~ColorManager(){}

  virtual size_t ElementsPerEntry() override { return 3; }
  virtual bool isFloating() { return true; }

  class Interface {
  public:
    void FillData(VectorSlice<Type>& data) { FillColorData(data); }
	  virtual void FillColorData(Data::VectorSlice<Type>& data) const = 0;
  };
};

class IndexManager : public BaseManager<IndexManager> {
public:
  using Type = uint32_t;
  static constexpr char key[] = "pass";

  IndexManager(size_t idx) : BaseManager<IndexManager>(idx) {}
  virtual ~IndexManager(){}

  virtual size_t ElementsPerEntry() override { return 3; }
  virtual bool isFloating() { return false; }

  class Interface {
  public:
    void FillData(VectorSlice<Type>& data) { FillIndexData(data); }
	  virtual void FillIndexData(Data::VectorSlice<Type>& data) const = 0;
  };
};


class VertexManager : public BaseManager<VertexManager> {
public:
  using Type = float;
  static constexpr char key[] = "inPosition";

  VertexManager(size_t idx) : BaseManager<VertexManager>(idx) {}
  virtual ~VertexManager(){}

  virtual size_t ElementsPerEntry() override { return 3; }
  virtual bool isFloating() { return true; }

  class Interface {
  public:
    void FillData(VectorSlice<Type>& data) { FillVertexData(data); }
	  virtual void FillVertexData(Data::VectorSlice<Type>& data) const = 0;
  };
};

}} // ShapeShifter::Data

#endif /* DATA_BUFFERTYPES_H */

