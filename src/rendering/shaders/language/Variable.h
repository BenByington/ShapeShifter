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

#include <concepts>

#ifndef RENDERING_SHADERS_LANGUAGE_VARIABLE_H
#define RENDERING_SHADERS_LANGUAGE_VARIABLE_H

#include "rendering/shaders/language/Expression.h"
#include "rendering/shaders/language/Types.h"

namespace ShapeShifter::Rendering::Shaders {

// forward declare
class VariableFactory;

namespace Language {

/*
 * Named type for use in a ShaderProgram.  T will be the underlying type that
 * results in the GLSL code.  This class will provide normal c++ operators
 * as long as the underlying T classes support those same operators, though
 * the T classes only need dummy implementations as they will not be called.
 */
template <typename T>
class Variable : public Expression<T> {
  friend class ShapeShifter::Rendering::Shaders::VariableFactory;
  Variable(IndentedStringStream& stream, const std::string& name)
    : Expression<T>(stream, name, Expression<T>::Key()) {}
public:
  using Base = Expression<T>;

  Variable(const Variable&) = delete;
  Variable(Variable&&) = default;

  ~Variable() {
    // We don't want to output the variable name if we're being destroyed
    // because of end-of-scope or something
    this->state_.clear();
  }

  // Note that we are explicitly not returning a reference to ourself.  The
  // return type either needs to be captured by another operator, so the
  // information in the state_ variable can be slurped up, or it needs to be
  // destroyed so the information in the state_ variable can be output as a
  // line in the program.
  template <std::convertible_to<T> U>
  Expression<T> operator=(Expression<U>&& other) {
    // ISSUE precidence for other operators?
    std::string result = this->state_ + " = " + other.state_;
    other.state_.clear();
    return Expression<T>(this->stream_, result, Base::Key());
  }

  // See above function for comment on why we're not returning *this;
  Expression<T> operator=(const Variable<T>& other) {
    std::string result = this->state_ + " = " + other.state_;
    return Expression<T>(this->stream_, result, Base::Key());
  }

  // Little bit of trickery here.  For Expression<T> classes, the state_
  // variable holds the line we are building up to output, and each operator
  // will suck out the state from what it operates on to add more to it.  When
  // destroyed the state_ variable will be output to the program, so intermediate
  // temporaries need their state cleared before they are destroyed.  However,
  // Variable<T> types use this field to store their variable name, and need to
  // avoid being cleared every time they are used, thus the noop here.
  virtual void clear_state() const {}
private:
};

}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_VARIABLE_H */

