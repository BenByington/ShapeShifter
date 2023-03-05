/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   InterfaceVariableBase.cpp
 * Author: bbyington
 *
 * Created on October 24, 2016, 8:18 AM
 */

#include "rendering/shaders/InterfaceVariable.h"

using namespace ShapeShifter::Rendering::Shaders;
// ISSUE move to test, maybe make these runtime failures?
namespace {

// Can't define this locally because of the static member.  I'll
// inherit from it instead so that I can keep the classes local to the
// tests.
template <bool b>
struct StaticSmooth {
  static constexpr bool smooth = b;
};

void TestInterfaceVariableConcept() {
  struct NoSmooth : Language::Variable<Language::Vec3> {
    using Base = Language::Variable<Language::Vec3>;
    NoSmooth(VariableFactory& factory)
        : Base(factory.create<Language::Vec3>(name())) {}
    static constexpr const char* name() { return "inColor"; }
  };
  static_assert(!InterfaceVariable<NoSmooth>);

  struct NoName : Language::Variable<Language::Vec3>, StaticSmooth<false> {
    using Base = Language::Variable<Language::Vec3>;
    NoName(VariableFactory& factory)
        : Base(factory.create<Language::Vec3>("")) {}
  };
  static_assert(!InterfaceVariable<NoName>);

  struct NoParent : StaticSmooth<false> {
    static constexpr const char* name() { return "inColor"; }
  };
  static_assert(!InterfaceVariable<NoParent>);

  struct WrongType : Language::Variable<int>, StaticSmooth<false> {
    using Base = Language::Variable<int>;
    WrongType(VariableFactory& factory)
        : Base(factory.create<int>(name())) {}
    static constexpr const char* name() { return "inColor"; }
  };
  static_assert(!InterfaceVariable<WrongType>);

  struct GoodVariable : Language::Variable<Language::Vec3>, StaticSmooth<false> {
    using Base = Language::Variable<Language::Vec3>;
    GoodVariable(VariableFactory& factory)
        : Base(factory.create<Language::Vec3>(name())) {}
    static constexpr const char* name() { return "inColor"; }
  };
  static_assert(InterfaceVariable<GoodVariable>);
}

} // namespace
