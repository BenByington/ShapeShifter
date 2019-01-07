/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Sphere.h
 * Author: bbyington
 *
 * Created on August 31, 2016, 4:07 PM
 */

#ifndef SHAPES_SPHERE_H
#define SHAPES_SPHERE_H

#include "data/ConcreteBufferManager.h"
#include "rendering/PureNode.h"

#include <cmath>

namespace ShapeShifter {
namespace Shapes {

class Sphere final : public Rendering::LeafNode<
    Data::VertexManager,
    Data::ColorManager,
    Data::NormalManager
>{
  using BufferIndex = Data::BufferIndex;

public:
  Sphere(float radius);
  Sphere(const Sphere& orig) = delete;
  Sphere(Sphere&& orig) = delete;
  Sphere& operator=(const Sphere& orig) = delete;

  ~Sphere() {}
private:

  virtual BufferIndex ExclusiveNodeDataCount() const override;
  virtual void FillVertexData(Data::VectorSlice<float>& data) const override;
  virtual void FillNormalData(Data::VectorSlice<float>& data) const override;
  virtual void FillColorData(Data::VectorSlice<float>& data) const override;
  virtual void FillIndexData(Data::VectorSlice<uint32_t>& data) const override;
  virtual void DrawSelf() const override;

  // ISSUE: This isn't quite the radius...
  float radius_;
};

}} // ShapeShifter::Shapes

#endif /* SHAPES_SPHERE_H */

