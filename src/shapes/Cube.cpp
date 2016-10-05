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

#include <iostream>

namespace ShapeShifter {
namespace Shapes {

Cube::Cube(float sx, float sy, float sz) : sx_(sx), sy_(sy), sz_(sz) {}

size_t Cube::ExclusiveNodeVertexCount() const {
  return 72;
}

void Cube::FillVertexData(Opengl::VectorSlice<float>&& data) const {
  auto FillVertex = [&](float f1, float f2, float f3) {
    static size_t idx = 0;
    data[idx] = f1;
    data[idx+1] = f2;
    data[idx+2] = f3;
    idx += 3;
  };

  auto FillFace = [&](const size_t dim, const bool pos) {
    const auto d1 = dim == 0 ? 1 : 0;
    const auto d2 = dim == 2 ? 1 : 2;

    const auto forward_perm = std::array<float, 12> {
      0.0f, 0.0f,
      1.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 1.0f
    };

    const auto reverse_perm = std::array<float, 12> {
      1.0f, 0.0f,
      0.0f, 1.0f,
      0.0f, 0.0f,
      1.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f
    };

    const auto& perm = pos ? forward_perm : reverse_perm;

    auto point = std::array<float, 3> {};
    for (size_t i = 0; i < 12; i+=2) {
      point[d1] = perm[i];
      point[d2] = perm[i+1];
      point[dim] = pos ? 0 : 1;

      FillVertex(sx_ * point[0], sy_ * point[1], sz_ * point[2]);
    }

  };

  FillFace(0, true);
  FillFace(1, true);
  FillFace(2, true);
  FillFace(0, false);
  FillFace(1, false);
  FillFace(2, false);
}

void Cube::FillColorData(Opengl::VectorSlice<float>&& data) const {
  auto FillFaceColor = [&](float f1, float f2, float f3) {
    static size_t idx = 0;
    for (size_t i = 0; i < 6; ++i) {
      data[idx] = f1;
      data[idx+1] = f2;
      data[idx+2] = f3;
      idx+=3;
    }
  };

  FillFaceColor(1, 0, 1);
  FillFaceColor(0, 0, 1);
  FillFaceColor(0, 1, 0);
  FillFaceColor(1, 0, 0);
  FillFaceColor(1, 1, 0);
  FillFaceColor(0, 1, 1);
}

void Cube::DrawSelf() const {
  glDrawArrays (GL_TRIANGLES, start_vertex(), this->ExclusiveNodeVertexCount());
}

}} // ShapeShifter::Shapes