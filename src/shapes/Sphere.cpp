/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Sphere.cpp
 * Author: bbyington
 *
 * Created on August 31, 2016, 4:07 PM
 */

#include "Sphere.h"

#include <iostream>

namespace ShapeShifter {
namespace Shapes {

Sphere::Sphere(float radius) : radius_(radius) {}

size_t Sphere::ExclusiveNodeVertexCount() const {
  return 12;
}

size_t Sphere::ExclusiveNodeIndexCount() const {
  return 60;
}

void Sphere::FillVertexData(Opengl::VectorSlice<float>& data) const {
  // TODO unify with cube's version and build into slice.  It can automatically
  // know how many floats go with a single vertex for each buffer.
  auto FillVertex = [&](float f1, float f2, float f3) {
    static size_t idx = 0;
    data[idx] = radius_* f1;
    data[idx+1] = radius_*f2;
    data[idx+2] = radius_*f3;
    idx += 3;
  };

  FillVertex(phi, 0, 1);
  FillVertex(phi, 0, -1);
  FillVertex(1, phi, 0);
  FillVertex(1, -phi, 0);

  FillVertex(0, 1, phi);
  FillVertex(0, -1, phi);

  FillVertex(-1, -phi, 0);

  FillVertex(0, -1, -phi);
  FillVertex(0, 1, -phi);

  FillVertex(-1, phi, 0);

  FillVertex(-phi, 0, 1);
  FillVertex(-phi, 0, -1);
}

void Sphere::FillColorData(Opengl::VectorSlice<float>& data) const {
  // TODO unify with cube's version?
  auto FillColor = [&](float f1, float f2, float f3) {
    static size_t idx = 0;
    data[idx] = f1;
    data[idx+1] = f2;
    data[idx+2] = f3;
    idx += 3;
  };

  FillColor(0, 0, 1);
  FillColor(0, 1, 0);
  FillColor(0, 1, 1);
  FillColor(1, 0, 0);

  FillColor(1, 0, 1);
  FillColor(1, 1, 0);
  FillColor(1, 1, 1);
  FillColor(0, 0, 1);

  FillColor(0, 0, 1);
  FillColor(0, 1, 0);
  FillColor(1, 0, 0);
  FillColor(1, 0, 1);
}

void Sphere::FillIndexData(Opengl::VectorSlice<uint32_t>& data) const {
  // TODO unify with cube's version?
  auto FillTriangle = [&](float f1, float f2, float f3) {
    static size_t idx = 0;
    data[idx] = f1;
    data[idx+1] = f2;
    data[idx+2] = f3;
    idx += 3;
  };

  // TODO replace with something more procedural
  // Should port over mesh tesselation code.
  FillTriangle(0,1,2);
  FillTriangle(0,3,1);

  FillTriangle(0,2,4);
  FillTriangle(0,4,5);
  FillTriangle(0,5,3);

  FillTriangle(3, 5, 6);
  FillTriangle(3, 6, 7);
  FillTriangle(3, 7, 1);

  FillTriangle(1, 7, 8);
  FillTriangle(1, 8, 2);

  FillTriangle(2, 8, 9);
  FillTriangle(2, 9, 4);

  FillTriangle(4, 9, 10);
  FillTriangle(4, 10, 5);

  FillTriangle(5, 10, 6);

  FillTriangle(11, 6, 10);
  FillTriangle(11, 10, 9);
  FillTriangle(11, 9, 8);
  FillTriangle(11, 8, 7);
  FillTriangle(11, 7, 6);
}

void Sphere::DrawSelf() const {
  //TODO set up global and toggle-able culling
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);

  glDrawElements(
      GL_TRIANGLES,
      ExclusiveNodeIndexCount(),
      GL_UNSIGNED_INT,
      (GLvoid*)(start_index()*sizeof(uint32_t)));
}

}} // ShapeShifter::Shapes