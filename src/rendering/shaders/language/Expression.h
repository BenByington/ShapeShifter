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
namespace Language {

template <typename T>
class Expression {
public:
  //  TODO can this be protected somehow?  Child class needs to be able to
  //  create instances, but for some reason I can't.
  Expression(IndentedStringStream& stream, const std::string& name)
    : stream_(stream)
    , state_(name) {}

  // TODO make this less centralized...
  static constexpr const char* TypeName() {
    if (std::is_same<float, T>::value) {
      return "float";
    } else if (std::is_same<int, T>::value) {
      return "int";
    } else if (std::is_same<Vec3, T>::value) {
      return "vec3";
    } else if (std::is_same<Vec4, T>::value) {
      return "vec4";
    } else if (std::is_same<Mat4, T>::value) {
      return "mat4";
    } else {
      return "Monsters in the soup!\n";
    }
  }

  // TODO: Make this safer somehow?
  // TODO: kill mutable!
  mutable std::string state_;
  virtual void clear_state() const { state_ = ""; }

protected:
  Expression(const Expression&) = delete;
  Expression& operator=(const Expression&) = delete;
  Expression& operator=(Expression&&) = delete;

  std::reference_wrapper<IndentedStringStream> stream_;

public:
  Expression(Expression&&) = default;
  virtual ~Expression() {
    if(!state_.empty()) stream_.get() << state_ << ";" << std::endl;
  }

  // TODO need to handle precidence!  Right now, a * (b+c) will get written as
  // a * b + c;
  template <typename U>
  auto operator*(const Expression<U>& other) -> Expression<decltype(std::declval<T>()*std::declval<U>())> {
    using Type = Expression<decltype(std::declval<T>()*std::declval<U>())>;
    std::string result = state_ + " * " + other.state_;
    other.clear_state();
    return Type(this->stream_, result);
  }
};



}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_EXPRESSION_H */

