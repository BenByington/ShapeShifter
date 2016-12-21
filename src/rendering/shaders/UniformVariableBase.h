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

template <class Child, typename T>
class UniformVariableBase : public InterfaceVariableBase<Child, T> {
  using Base = InterfaceVariableBase<Child, T>;
public:
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

