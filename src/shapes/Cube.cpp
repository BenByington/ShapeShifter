/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Cube.cpp
 * Author: bbyington
 *
 * Created on August 31, 2016, 4:07 PM
 */

#include "Cube.h"

#include "opengl/data/BufferTypes.h"

#include <iostream>

namespace ShapeShifter {
namespace Shapes {

using namespace Opengl::Data;

Cube::Cube(float sx, float sy, float sz) : sx_(sx), sy_(sy), sz_(sz) {}

BufferIndex Cube::ExclusiveNodeDataCount() const {
  auto ret = BufferIndex();
  ret.triangle_ = 12;
  ret.vertex_ = 36;
  return ret;
}

void Cube::FillVertexData(VectorSlice<float>& data) const {
  auto DataFiller = data.Filler();

  auto FillFace = [&](const size_t dim, const bool pos) {
    const auto d1 = dim == 0 ? 1 : 0;
    const auto d2 = dim == 2 ? 1 : 2;

    const auto forward_perm = std::array<float, 12> {
      0.0f, 0.0f,
      1.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 0.0f,
      1.0f, 1.0f,
      0.0f, 1.0f
    };

    const auto reverse_perm = std::array<float, 12> {
      1.0f, 0.0f,
      0.0f, 1.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 0.0f,
      1.0f, 1.0f
    };

    const auto& perm = pos ? forward_perm : reverse_perm;

    auto point = std::array<float, 3> {};
    for (size_t i = 0; i < 12; i+=2) {
      point[d1] = perm[i];
      point[d2] = perm[i+1];
      point[dim] = pos ? 0 : 1;

      DataFiller(sx_ * point[0], sy_ * point[1], sz_ * point[2]);
    }

  };

  FillFace(0, true);
  FillFace(1, true);
  FillFace(2, true);
  FillFace(0, false);
  FillFace(1, false);
  FillFace(2, false);
}

void Cube::FillColorData(VectorSlice<float>& data) const {
  auto DataFiller = data.Filler();
  auto idx = size_t{0};
  auto FillFaceColor = [&](float f1, float f2, float f3) {
    for (size_t i = 0; i < 6; ++i) {
      DataFiller(f1, f2, f3);
    }
  };

  FillFaceColor(1, 0, 1);
  FillFaceColor(0, 0, 1);
  FillFaceColor(0, 1, 0);
  FillFaceColor(1, 0, 0);
  FillFaceColor(1, 1, 0);
  FillFaceColor(0, 1, 1);
}

void Cube::FillIndexData(VectorSlice<uint32_t>& data) const {
  for (uint32_t i = 0; i < ExclusiveNodeDataCount().vertex_; ++i) {
    data[i] = i;
  }
}

void Cube::DrawSelf() const {
  glDrawElements(
      GL_TRIANGLES,
      ExclusiveNodeDataCount().triangle_*floats_per_triangle,
      GL_UNSIGNED_INT,
      // TODO look for cleaner cast?
      //TODO make uint32_t configurable
      (GLvoid*)(start().triangle_*floats_per_triangle*sizeof(uint32_t)));
}

}} // ShapeShifter::Shapes