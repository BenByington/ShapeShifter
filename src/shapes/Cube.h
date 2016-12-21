/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Cube.h
 * Author: bbyington
 *
 * Created on August 31, 2016, 4:07 PM
 */

#ifndef SHAPES_CUBE_H
#define SHAPES_CUBE_H

#include "rendering/PureNode.h"
#include "data/ConcreteBufferManager.h"

namespace ShapeShifter {
namespace Shapes {

class Cube final : public Rendering::LeafNode<
    Data::VertexManager,
    Data::ColorManager
>{

  using BufferIndex = Data::BufferIndex;
public:
  Cube(float sx, float sy, float sz);
  Cube(const Cube& orig) = delete;
  Cube(Cube&& orig) = delete;
  Cube& operator=(const Cube& orig) = delete;

  ~Cube() {}
private:

  virtual BufferIndex ExclusiveNodeDataCount() const override;
	virtual void FillVertexData(Data::VectorSlice<float>& data) const override;
	virtual void FillColorData(Data::VectorSlice<float>& data) const override;
	virtual void FillIndexData(Data::VectorSlice<uint32_t>& data) const {}
  virtual void DrawSelf() const override;

  float sx_;
  float sy_;
  float sz_;
};

}} // ShapeShifter::Shapes

 #endif /*SHAPES_CUBE_H */

