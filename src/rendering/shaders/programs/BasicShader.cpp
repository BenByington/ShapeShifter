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

void BasicVertexShader::DefineMain() {
  gl_Position = transform * Language::Vec4(inPosition, 1.0f);
  theColor = inColor;
}

void BasicFragmentShader::DefineMain() {
  outputColor = Language::Vec4(theColor, 1.0f);
}

void PhongVertexShader::DefineMain() {
  gl_Position = transform * Language::Vec4(inPosition, 1.0f);
  theColor = inColor;
  theNormal = inNormal;
  thePos = inPosition;
}

void PhongFragmentShader::DefineMain() {
  Language::Vec3 diff = lightPos - thePos;
  //outputColor = Language::Vec4((max(dot(normalize(lightPos - thePos)), theNormal), 0.0f) + ambientLight) * lightColor * theColor, 1.0f);
}

}}}} // ShapeShifter::Rendering::Shaders::Programs
