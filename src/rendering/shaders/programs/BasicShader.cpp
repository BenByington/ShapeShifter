/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BasicShader.cpp
 * Author: bbyington
 *
 * Created on November 1, 2016, 4:26 PM
 */

#include "BasicShader.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Programs {

void BasicVertexShader::DefineMain(const VariableFactory& factory) {
  gl_Position = transform * factory_.temporary<Language::Vec4>(inPosition, 1.0f);
  theColor = inColor;
}

void BasicFragmentShader::DefineMain(const VariableFactory& factory) {
  outputColor = factory_.temporary<Language::Vec4>(theColor, 1.0f);
}

void PhongVertexShader::DefineMain(const VariableFactory& factory) {
  gl_Position = transform * factory_.temporary<Language::Vec4>(inPosition, 1.0f);
  theColor = inColor;
  theNormal = inNormal;
  thePos = inPosition;
}

void PhongFragmentShader::DefineMain(const VariableFactory& factory) {
  outputColor =factory_.temporary<Language::Vec4>((max(dot(normalize(lightPos - thePos), factory_.temporary<Language::Vec3>(theNormal)), 0.0f) + ambientLight) * lightColor * theColor, 1.0f);
  //Language::Vec3 v = lightPos - this->theNormal;
  //outputColor = ambientLight * factory_.temporary<Language::Vec4>(theColor, 1.0f);
}

}}}} // ShapeShifter::Rendering::Shaders::Programs
