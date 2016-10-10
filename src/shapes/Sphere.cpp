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

#include "data/BufferTypes.h"

#include <iostream>

namespace ShapeShifter {
namespace Shapes {

using namespace Data;

Sphere::Sphere(float radius) : radius_(radius) {}

BufferIndex Sphere::ExclusiveNodeDataCount() const {
  auto ret = BufferIndex();
  ret.vertex_ = 12;
  ret.triangle_ = 20;
  return ret;
}

void Sphere::FillVertexData(VectorSlice<float>& data) const {
  auto data_filler = data.Filler();
  auto FillVertex = [&](float f1, float f2, float f3) {
    data_filler(radius_*f1, radius_*f2, radius_*f3);
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

void Sphere::FillColorData(VectorSlice<float>& data) const {
  auto DataFiller = data.Filler();

  DataFiller(0, 0, 1);
  DataFiller(0, 1, 0);
  DataFiller(0, 1, 1);
  DataFiller(1, 0, 0);

  DataFiller(1, 0, 1);
  DataFiller(1, 1, 0);
  DataFiller(1, 1, 1);
  DataFiller(0, 0, 1);

  DataFiller(0, 0, 1);
  DataFiller(0, 1, 0);
  DataFiller(1, 0, 0);
  DataFiller(1, 0, 1);
}

void Sphere::FillIndexData(VectorSlice<uint32_t>& data) const {
  auto FillData = data.Filler();

  //ISSUE: Enable resolution adjusting, and put in a more procedural generation
  FillData(0,1,2);
  FillData(0,3,1);

  FillData(0,2,4);
  FillData(0,4,5);
  FillData(0,5,3);

  FillData(3, 5, 6);
  FillData(3, 6, 7);
  FillData(3, 7, 1);

  FillData(1, 7, 8);
  FillData(1, 8, 2);

  FillData(2, 8, 9);
  FillData(2, 9, 4);

  FillData(4, 9, 10);
  FillData(4, 10, 5);

  FillData(5, 10, 6);

  FillData(11, 6, 10);
  FillData(11, 10, 9);
  FillData(11, 9, 8);
  FillData(11, 8, 7);
  FillData(11, 7, 6);
}

void Sphere::DrawSelf() const {
  //ISSUE set up global and toggle-able culling
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);

  glDrawElements(
      GL_TRIANGLES,
      ExclusiveNodeDataCount().triangle_*floats_per_triangle,
      GL_UNSIGNED_INT,
      StartIndexAsVP());
}

}} // ShapeShifter::Shapes