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
#include "data/BufferMapBase.h"
#include "rendering/shaders/InterfaceVariableBase.h"

#include <cstdlib>
#include <type_traits>

namespace ShapeShifter {
namespace Data {

class ColorManager final : public AbstractManager {
public:
  using Type = float;
  using Type2 = Rendering::Shaders::Language::Vec3;

  virtual ~ColorManager(){}

  virtual size_t ElementsPerEntry() const override { return 3; }
  virtual bool isFloating() const override { return true; }
  virtual std::string name() const override { return Variable::name(); }

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

class VertexManager final : public AbstractManager {
public:
  using Type = float;
  using Type2 = Rendering::Shaders::Language::Vec3;
  virtual ~VertexManager(){}

  virtual size_t ElementsPerEntry() const override { return 3; }
  virtual bool isFloating() const override { return true; }
  virtual std::string name() const override { return Variable::name(); }

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

