/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Variable.h
 * Author: bbyington
 *
 * Created on November 15, 2016, 8:45 AM
 */

#ifndef RENDERING_SHADERS_LANGUAGE_VARIABLE_H
#define RENDERING_SHADERS_LANGUAGE_VARIABLE_H

#include "rendering/shaders/language/Expression.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

// forward declare
class VariableFactory;

namespace Language {

template <typename T>
class Variable : public Expression<T> {
  friend class ShapeShifter::Rendering::Shaders::VariableFactory;
  Variable(IndentedStringStream& stream, const std::string& name)
    : Expression<T>(stream, name, Expression<T>::Key()) {}
public:
  using Base = Expression<T>;

  Variable(const Variable&) = delete;
  Variable(Variable&&) = default;

  // Allow any implicit conversions?
  template <typename U>
  typename std::enable_if<std::is_same<U,T>::value, Expression<T>>::type
  operator=(Expression<U>&& other) {
    // ISSUE precidence for other operators?
    std::string result = this->state_ + " = " + other.state_;
    other.state_.clear();
    return Expression<T>(this->stream_, result, Base::Key());
  }

  Expression<T> operator=(const Variable<T>& other) {
    std::string result = this->state_ + " = " + other.state_;
    return Expression<T>(this->stream_, result, Base::Key());
  }

  virtual void clear_state() const {}
private:
};

}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_VARIABLE_H */

