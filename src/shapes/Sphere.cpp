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

void Sphere::FillVertexData(Opengl::Data::VectorSlice<float>& data) const {
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

// TODO search for and eradicate long fully qualified names
void Sphere::FillColorData(Opengl::Data::VectorSlice<float>& data) const {
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

void Sphere::FillIndexData(Opengl::Data::VectorSlice<uint32_t>& data) const {
  auto FillData = data.Filler();

  // TODO replace with something more procedural
  // Should port over mesh tesselation code.
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