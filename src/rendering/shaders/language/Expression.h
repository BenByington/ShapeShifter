/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Expression.h
 * Author: bbyington
 *
 * Created on November 15, 2016, 8:32 AM
 */

#ifndef RENDERING_SHADERS_LANGUAGE_EXPRESSION_H
#define RENDERING_SHADERS_LANGUAGE_EXPRESSION_H

#include "rendering/shaders/language/IndentedStringStream.h"
#include "rendering/shaders/language/Types.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

// forward declare
class VariableFactory;

namespace Language {

// Sole purpose of base class is to provide the Key function so that
// sibling classes can create each other, but external code cannot
// instantiate new objects (save for the VariableFactor that has friend
// status)
class ExpressionBase {
private:
  ExpressionBase() {}
protected:
  static ExpressionBase Key() {
    return ExpressionBase{};
  }
public:
  ExpressionBase(ExpressionBase&&) = default;
};

template <typename T>
class Expression : private ExpressionBase{
  friend class ShapeShifter::Rendering::Shaders::VariableFactory;
public:
  Expression(IndentedStringStream& stream, const std::string& name, ExpressionBase b)
    : ExpressionBase(std::move(b))
    , stream_(stream)
    , state_(name) {}

  static constexpr const char* TypeName() {
    return VariableTraits<T>::name();
  }

  using Type = T;

  std::string state_;
  virtual void clear_state() { state_ = ""; }

protected:
  Expression(const Expression&) = delete;
  Expression& operator=(const Expression&) = delete;
  Expression& operator=(Expression&&) = delete;

  using ExpressionBase::Key;

  std::reference_wrapper<IndentedStringStream> stream_;

public:
  Expression(Expression&&) = default;
  virtual ~Expression() {
    if(!state_.empty()) stream_.get() << state_ << ";" << std::endl;
  }

  // ISSUE need to handle precidence!  Right now, a * (b+c) will get written as
  // a * b + c;
  template <typename U>
  auto operator*(U&& other) -> Expression<decltype(std::declval<T>()*std::declval<typename U::Type>())> {
    using Type = Expression<decltype(std::declval<T>()*std::declval<typename U::Type>())>;
    std::string result = state_ + " * " + other.state_;
    other.clear_state();
    return Type(this->stream_, result, Key());
  }
};



}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_EXPRESSION_H */

